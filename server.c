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

#define SLEEP_TIME 5 /*make sure to change to 20 before submission */

/* Global Variables */
AccountStoragePtr ACCOUNTS;

/* Thread that runs every twenty seconds,
which prints the account information*/
void *writeAccountsEveryTwentySeconds(void *arg)
{
  int account_index;

  while(1){
    sleep(SLEEP_TIME);
    if(ACCOUNTS->accounts[0] != NULL){
      for(account_index = 0; account_index <= MAX_ACCOUNTS; account_index++){
        printf("Name: %s \n", ACCOUNTS->accounts[account_index]->name);
        printf("Balance: %f \n", ACCOUNTS->accounts[account_index]->balance);
        printf("IN SESSION: %i \n\n\n", ACCOUNTS->accounts[account_index]->in_session);
      }
    }
    printf("Sup Simple Simon \n");
  }
  pthread_exit(NULL);
}

char** tokenizeCommand(char* buffer)
{
  char* tmp_buffer = buffer;
  char* first_word;
  char* second_word;
  char** product;
  const char delimiter[2] = " ";
  char* tmp_token;

  int i;

  product = malloc(sizeof(tmp_buffer));

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

  product[0] = first_word;
  product[1] = second_word;

  return product;
}

ClientRequestPtr getCommandFromBuffer(char* buffer)
{
  char* tmp_buffer = buffer;
  char* word;
  int nt_index;
  int i;
  char** client_command;
  ClientRequestPtr structured_client_information;

  client_command = malloc(sizeof(tmp_buffer)*2);
  structured_client_information = malloc(sizeof(struct client_request));

  nt_index = 0;
  while(tmp_buffer[nt_index] != '\0'){
    nt_index += 1;
  }

  i = 0;
  word = malloc(sizeof(tmp_buffer));
  while(i < nt_index - 1){
    word[i] = tmp_buffer[i];
    i += 1;
  }

  client_command = tokenizeCommand(tmp_buffer);

  structured_client_information->command = malloc(sizeof(tmp_buffer));
  structured_client_information->command = client_command[0];

  structured_client_information->argument = malloc(sizeof(tmp_buffer));
  structured_client_information->argument = client_command[1];


  return structured_client_information;

}

void handleClientCommand(char* command, char* argument) {

  if( argument == NULL) {
    if( strcmp(command, "query") == 0) {

    }else if( strcmp(command, "end") == 0) {

    }else if( strcmp(command, "quit") == 0) {

    }
  }else{

    if( strcmp(command, "create") == 0) {


    }else if( strcmp(command, "serve") == 0) {

    } else if( strcmp(command, "deposit") == 0) {

    } else if( strcmp(command, "withdraw") == 0) {

    }
  }
}

void createClientServiceThread(void * params)
{
  int timer_thread_tid;
  pthread_t timer_thread;
  SockInfo cs_sockinfo = (SockInfo) params;
  int n;
  char buffer[256];
  ClientRequestPtr client_information;

  timer_thread_tid = pthread_create(&timer_thread, NULL,
    (void*(*)(void*))writeAccountsEveryTwentySeconds, NULL);

  printf("In cs thread: %i \n", cs_sockinfo->sockfd);

  n = write(cs_sockinfo->sockfd, "Connected to server. Ready for commands", 255);
  if( n < 0) {
    error("ERROR on writing to socket");
  }

  bzero(buffer, 256);

  n = read(cs_sockinfo->sockfd, buffer, 255);
  /* find out what the buffer holds */
  client_information = getCommandFromBuffer(buffer);
  printf("%s %lu \t %s %lu \n", client_information->command,
    strlen(client_information->command),
    client_information->argument,
    strlen(client_information->argument));

  /* THIS IS WHERE WE GOTTA DO SHIT
  while(client_command != NULL) {
    handleClientCommand(client_command, client_command_argument);

  }
  */

  if( n < 0){
    error("ERROR reading from socket \n");
  }

  close(cs_sockinfo->sockfd);
  pthread_join(timer_thread, NULL);
  pthread_exit(NULL);
}

void createSessionAcceptorThread(void* params)
{
   UserArgs args = (UserArgs)params;

   //TCP/IP server logic
   int sockfd, portno;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   /* Confirming that number of arguments is valid */
   if( args->argc < 2 ){
      fprintf(stderr, "ERROR, no port provided \n");
      pthread_exit(NULL);
   }

   /* Creating a socket */
   sockfd = socket( AF_INET, SOCK_STREAM, 0 );
   if( sockfd < 0 ){
      error("ERROR opening socket");
   }

   /* Binding a socket and the server address */
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(args->argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons( portno );
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

   while(conn_count <= SOMAXCONN){

      listen(sockfd, SOMAXCONN);
      clilen = sizeof(cli_addr);
      SockInfo cs_sockinfo = malloc(sizeof(SockInfo));
      cs_sockinfo->sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if(cs_sockinfo->sockfd < 0){
        error("ERROR on accept");
      }
      tids[conn_count] = pthread_create( &threads[conn_count],
        NULL, (void*(*)(void*))createClientServiceThread, (void*)cs_sockinfo);
      conn_count += 1;
   }
   close(sockfd);
   pthread_join(threads[conn_count], NULL);
   pthread_exit(NULL);
}


int main(int argc, char** argv){

  ACCOUNTS = (AccountStoragePtr) malloc(sizeof(struct account_storage));
  //Session Acceptor Thread
  pthread_t thread;
  int rc, i;
  i = 0;

  UserArgs test_obj = malloc(sizeof(UserArgs));
  test_obj->argc = argc;
  test_obj->argv = argv;

  rc = pthread_create( &thread, NULL, (void*(*)(void*))createSessionAcceptorThread, (void* )test_obj);

  if( rc != 0 ){
     printf("pthread_create failed \n");
     return 0;
  }

  pthread_join(thread, NULL);
  return 0;
}
