#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* User created libraries */
#include "accounts.h"

struct user_args{
   int argc;
   char **argv;
};
typedef struct user_args* UserArgs;


struct sock_info{
  int sockfd;
};
typedef struct sock_info* SockInfo;

void error(const char *msg)
{
   perror(msg);
   exit(1);
}


/* Global Variables */
AccountStoragePtr ACCOUNTS; 
ACCOUNTS = malloc(sizeof(AccountStoragePtr));

void print_accounts()
{
   int i;
   AccountStoragePtr accounts = ACCOUNTS;
   for(i = 0; i < accounts.length(); i++){
     Account tmpAccount = accounts[i];
     printf("Account Name: %s \n", tmpAccount->name);
     printf("Balance: %f \n", tmpAccount->balance);
     if(tmpAccount->in_session == 1){
        printf("IN SESSION \n");
     }
   }
}


void client_service_thread(void* params)
{
  
  SockInfo cs_sockinfo = (SockInfo) params;
  int n;
  
  printf("In cs thread: %i ", cs_sockinfo->sockfd);

  n = write(cs_sockinfo->sockfd, "What's the message?", 18);
  if( n < 0) {
    error("ERROR on writing to socket");
  }

  char buffer[256];
  bzero(buffer, 256);
  n = read(cs_sockinfo->sockfd, buffer, 255);
  if( n < 0){
    error("ERROR reading from socket \n");
  }

  close(cs_sockinfo->sockfd);
  pthread_exit(NULL);
}

void session_acceptor_thread(void* params)
{
   UserArgs args = (UserArgs)params;
   printf("Session Acceptor Thread Created! \n");
   printf("argc : %d \t", args->argc);
   printf("argv: %s \n", args->argv[1]);

   //TCP/IP server logic
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;
   int n;

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
        NULL, (void *)client_service_thread, (void*)cs_sockinfo);

      conn_count += 1;
      printf("connection count: %i \n", conn_count);
   }
   close(sockfd);
   pthread_join(threads[conn_count], NULL);
   pthread_exit(NULL);
}


int main(int argc, char** argv){

  //Session Acceptor Thread
  pthread_t thread;
  int rc, i;
  i = 0;

  UserArgs test_obj = malloc(sizeof(UserArgs));
  printf("%d \t", argc);
  printf("%s \n", argv[1]);
  test_obj->argc = argc;
  test_obj->argv = argv;

  rc = pthread_create( &thread, NULL, (void *) session_acceptor_thread, (void* )test_obj);

  if( rc != 0 ){
     printf("pthread_create failed \n");
     return 0;
  }

  pthread_join(thread, NULL);
  return 0;
}
