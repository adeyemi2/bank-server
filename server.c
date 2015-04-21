#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/* User created libraries */
#include "accounts.h"
#include "server.h"


#define WITHDRAW "withdraw\0"
#define DEPOSIT "deposit\0"
#define SERVE "serve\0"
#define QUERY "query\0"
#define END "end\0"
#define QUIT "quit\0"
#define CREATE "create\0"
#define EMPTY_STRING ""
#define SLEEP_TIME 6 /* TODO make sure to change to 20 before submission */

/* Global Variables */
AccountStoragePtr ACCOUNTS;
pthread_t timer_thread;
pthread_mutex_t lock;
int acceptor_socket;
int* SOCKETS;

int killSocket(int sockfd)
{
  int socket_index;
  socket_index = 0;
  while( socket_index < MAX_ACCOUNTS) {
    if( SOCKETS[socket_index] == sockfd) {
      SOCKETS[socket_index] = 0;
      return 1;
    }
    socket_index++;
  }
  return 0;
}

int isThreadInSession(int thread, AccountStoragePtr all_accounts)
{
  int i;
  i = 0;
  // find the thread index
  while( i < MAX_ACCOUNTS ) {
    if( all_accounts->threads[i] == thread) {
      break;
    }
    i++;
  }
  // didn't find the thread
  if( i == MAX_ACCOUNTS && all_accounts->threads[i-1] != thread ) {
    return 0;
  }
  if( all_accounts->accounts[i] && all_accounts->accounts[i]->in_session ) {
    return 1;
  }
  return 0;

}
/*
Eliminate all sockets
*/
void shutdownServer() {
  int i, close_sockets_flag;
  printf("Closing server and all threads...\n");
  close_sockets_flag = closeAllSockets();
  if( close_sockets_flag == 1 ){
    i = 0;
    while(i < MAX_ACCOUNTS) {
      destroyAccount(ACCOUNTS->accounts[i]);
      i++;
    }
    free(ACCOUNTS);
    free(SOCKETS);
    close(acceptor_socket);
    exit(1);
  }
  if( close_sockets_flag == 0){
    error("ERROR Could not shutdown.");
  }
}

int closeAllSockets()
{
  int socket_index;
  int n;
  socket_index = 0;
  while(SOCKETS[socket_index] != 0){
    printf("Socket: %i being closed right now \n", SOCKETS[socket_index] );
    n = write(SOCKETS[socket_index], "quit", 4);
    if(n < 0)
      return 0;
    close(SOCKETS[socket_index]);
    socket_index += 1;
  }
  return 1;
}

void signal_handler(int signo)
{
  if(signo == SIGINT){
    shutdownServer();
  }
}


ClientResponsePtr handleClientCommand(int thread, ClientRequestPtr client_information)
{
  float balance;
  AccountPtr account;
  int success;
  char message[255];

  ClientResponsePtr return_value;
  return_value = malloc(sizeof(struct client_response));

  account = getThreadAccount(thread, ACCOUNTS);
  if(strcmp(client_information->argument, EMPTY_STRING) == 0){  
    if(strcmp(client_information->command, QUERY) == 0){

      if( !isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Not connected to an account");
        return return_value;
      }

      balance = accountGetBalance(thread,ACCOUNTS);
      return_value->balance = balance;
      return_value->is_query = 1;
      return_value->command_performed = 1;
      sprintf(message,"%f", balance);
      strcpy(return_value->message, message);
      bzero(message,255);
      return return_value;
    }

    if(strcmp(client_information->command, END) == 0){
      if( !isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Not connected to an account");
        return return_value;
      }
      accountEndConnection(thread, ACCOUNTS);
      return_value->balance = 1;
      return_value->is_query = 0;
      return_value->command_performed = 1;
      strcpy(return_value->message, "Session ended.");
      return return_value;
    }

    if(strcmp(client_information->command, QUIT) == 0){
      accountEndConnection(thread, ACCOUNTS);
      return_value->balance =-1;
      return_value->is_query = 0;
      return_value->command_performed = 1;
      return return_value;
    }
  } else {
    if(strcmp(client_information->command, WITHDRAW) == 0){
      if( !isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Not connected to an account");
        return return_value;
      }
      return_value->command_performed = 0;
      success = accountWithdraw(thread, atof(client_information->argument), ACCOUNTS);
      if( success == 1 ){
        return_value->command_performed = 1;
        strcpy(return_value->message, "Withdraw success.");
        return return_value;
      }
      strcpy(return_value->message, "Could not withdraw funds.");
      return return_value;

    }

    if(strcmp(client_information->command, DEPOSIT) == 0){
      if( !isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Not connected to an account");
        return return_value;
      }

      accountDeposit(thread, atof(client_information->argument), ACCOUNTS);
      strcpy(return_value->message, "Deposit success.");
      return_value->command_performed = 1;
      return return_value;
    }

    if(strcmp(client_information->command, CREATE) == 0){

      if( isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Cannot create account");
        return return_value;
      }

      account = accountCreate(client_information->argument, ACCOUNTS);
      if( account != NULL ) {
        return_value->command_performed = 1;
        strcpy(return_value->message, "Account created.");
        return return_value;
      }
      strcpy(return_value->message, "Account already exists.");
      return_value->command_performed = 0;
      return return_value;

    }
    if(strcmp(client_information->command, SERVE) == 0){

      // check if thread is already serving an account
      if( isThreadInSession(thread, ACCOUNTS)) {
        return_value->command_performed = 0;
        strcpy(return_value->message, "Already connected to an account");
        return return_value;
      }

      success = accountServe(thread, client_information->argument, ACCOUNTS);
      if( success == 1 ) {
        strcpy(return_value->message, "Serving account.");
        return_value->command_performed = 1;
        return return_value;
      }
      strcpy(return_value->message, "Account not found.");
      return_value->command_performed = 0;
      return return_value;
    }
  }
  strcpy(return_value->message, "Invalid command.");
  return_value->command_performed = 0;
  return return_value;
}

/* Thread that runs every twenty seconds,
which prints the account information*/

/* List

1. Write mutex stuff to this

*/
void *writeAccountsEveryTwentySeconds(void *arg)
{
  int account_index;

  while(1){
    sleep(SLEEP_TIME);
    pthread_mutex_lock(&lock);
    if(ACCOUNTS->accounts[0] != NULL){
      printf("Accounts: [\n");
      for(account_index = 0; account_index <= MAX_ACCOUNTS; account_index++){
        accountPrint(ACCOUNTS->accounts[account_index]);
      }
      printf("];\n");
    }

    // for(socket_index = 0; socket_index <= MAX_ACCOUNTS; socket_index++){
    //     printf("%i \n", SOCKETS[socket_index]);
    // }
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(NULL);
}
/*
Add socket to global array of sockets if not full
*/
int addSocket(int sockfd)
{
  int socket_index;

  socket_index = 0;
  while(SOCKETS[socket_index] != 0){
    socket_index += 1;
  }
  if(SOCKETS[socket_index] != 0)
    return 0;

  SOCKETS[socket_index] = sockfd;
  return 1;
}


ClientRequestPtr getCommandFromBuffer(char* buffer)
{
  char* tmp_buffer;
  char* first_word;
  char* second_word;
  char* tmp_token;
  int i;
  ClientRequestPtr structured_client_information;
  const char delimiter[2] = " ";

  tmp_buffer = buffer;
  structured_client_information = malloc(sizeof(struct client_request));

  first_word = malloc(sizeof(tmp_buffer));
  second_word = malloc(sizeof(tmp_buffer));

  /* First Word */
  tmp_token = malloc(sizeof(tmp_buffer));
  tmp_token = strtok( tmp_buffer, delimiter);

  i = 0;
  while( tmp_token != NULL ){
    if(i == 0){
      first_word = tmp_token;
    }
    if(i == 1){
      second_word = tmp_token;
      break;
    }
    tmp_token = strtok(NULL, delimiter);
    i += 1;
  }

  //structured_client_information->command = malloc(sizeof(client_command[0]));
  structured_client_information->command = first_word;

  //structured_client_information->argument = malloc(sizeof(client_command[1]));
  structured_client_information->argument = second_word;

  return structured_client_information;

}

void createClientServiceThread(void * params)
{

  SockInfo cs_sockinfo = (SockInfo) params;
  int n;
  char buffer[256];
  int kill_socket_in_array_flag;
  char message[255];
  ClientRequestPtr client_information;
  ClientResponsePtr handle_client_response;

  printf("In client service socket: %i \n", cs_sockinfo->sockfd);

  n = write(cs_sockinfo->sockfd, "Connected to server. Ready for commands", 255);
  if( n < 0) {
    error("ERROR on writing to socket");
  }

  bzero(buffer, 256);

  // Read commands from socket
  while( read(cs_sockinfo->sockfd, buffer, 255) > 0){
    client_information = getCommandFromBuffer(buffer);
    printf("Command: %s, %s \n", client_information->command,client_information->argument);

    if(strcmp(client_information->command, "quit") == 0) {
      break;
    }
    // send command to handler
    handle_client_response = handleClientCommand(cs_sockinfo->sockfd, client_information);

    if(handle_client_response->command_performed != 1) {
      printf("command not executed\n");
    }else{
      printf("command executed\n");
    }

    if( strlen(handle_client_response->message) >0) {
      sprintf(message,"'%s'", handle_client_response->message);
      write(cs_sockinfo->sockfd, message, 255);
    }

    free(handle_client_response);

    if(handle_client_response->is_query == 0 && handle_client_response->balance == -1){
      break;
    }
    bzero(buffer, 256);
    bzero(message, 255);
  }
  // deal with a quit signal

  accountEndConnection(cs_sockinfo->sockfd, ACCOUNTS); // end the connection, if one is open
  write(cs_sockinfo->sockfd, "quit", 4);
  kill_socket_in_array_flag = killSocket( cs_sockinfo->sockfd);
  if( kill_socket_in_array_flag == 0){
    error("ERROR in removing socket from SOCKETS array \n");
  }

  printf("Exiting socket %i\n", cs_sockinfo->sockfd);
  close(cs_sockinfo->sockfd);
  pthread_exit(NULL);
}

void createSessionAcceptorThread(void* params)
{
   //TCP/IP server logic
   int sockfd;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   /* Creating a socket */
   sockfd = socket( AF_INET, SOCK_STREAM, 0 );
   if( sockfd < 0 ){
      error("ERROR opening socket");
   }
   acceptor_socket = sockfd;
   /* Binding a socket and the server address */
   bzero((char *)&serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons( 3500 );
   if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ){
      error("ERROR on binding");
   }

   /* Now that we have a socket in place with its server address,
    * that for each client connection, we create a client service thread
    * that gets invoked
    */

   int conn_count = 0;
   pthread_t threads[SOMAXCONN];
   int tids[SOMAXCONN];
   int add_to_socket_flag = 0;

   while(conn_count <= SOMAXCONN){

      listen(sockfd, SOMAXCONN);
      clilen = sizeof(cli_addr);
      SockInfo cs_sockinfo = malloc(sizeof(SockInfo));
      cs_sockinfo->sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if(cs_sockinfo->sockfd < 0){
        error("ERROR on accept");
      }

      /* Add socket to sockets global array */
      add_to_socket_flag = addSocket(cs_sockinfo->sockfd);
      if(add_to_socket_flag == 0)
        error("ERROR on adding to sockets array");
      tids[conn_count] = pthread_create( &threads[conn_count],
        NULL, (void*(*)(void*))createClientServiceThread, (void*)cs_sockinfo);
      conn_count++;
   }
   close(sockfd);
   pthread_join(threads[conn_count], NULL);
   pthread_exit(NULL);
}

int main(int argc, char** argv){
  int timer_thread_tid;

  signal(SIGINT, signal_handler);

  ACCOUNTS = (AccountStoragePtr) malloc(sizeof(struct account_storage));
  SOCKETS = malloc(sizeof(int) * SOMAXCONN);
  ACCOUNTS->last_account_index = 0;
  //Session Acceptor Thread
  pthread_t thread;
  int rc, i;
  i = 0;

  UserArgs test_obj = malloc(sizeof(UserArgs));
  test_obj->argc = argc;
  test_obj->argv = argv;

  if( pthread_mutex_init(&lock, NULL) != 0 )
  {
    printf("\n mutex init failed \n");
    return 1;
  }

  timer_thread_tid = pthread_create(&timer_thread, NULL,
    (void*(*)(void*))writeAccountsEveryTwentySeconds, NULL);

  rc = pthread_create( &thread, NULL, (void*(*)(void*))createSessionAcceptorThread, (void* )NULL);

  if( rc != 0 ){
     printf("pthread_create failed \n");
     return 0;
  }

  pthread_join(timer_thread, NULL);
  pthread_join(thread, NULL);
  return 0;
}
