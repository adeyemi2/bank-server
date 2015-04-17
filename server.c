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
#include "server.h"


/* Global Variables */
AccountStoragePtr ACCOUNTS;
ACCOUNTS = malloc(sizeof(AccountStoragePtr));


void createClientServiceThread(void* params)
{

  SockInfo cs_sockinfo = (SockInfo) params;
  int n;
  char buffer[256];

  printf("In cs thread: %i ", cs_sockinfo->sockfd);

  n = write(cs_sockinfo->sockfd, "What's the message?", 18);
  if( n < 0) {
    error("ERROR on writing to socket");
  }

  bzero(buffer, 256);
  n = read(cs_sockinfo->sockfd, buffer, 255);
  printf("buffer %s", buffer);

  if( n < 0){
    error("ERROR reading from socket \n");
  }

  close(cs_sockinfo->sockfd);
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
        NULL, (void *)createClientServiceThread, (void*)cs_sockinfo);
      conn_count += 1;
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
  test_obj->argc = argc;
  test_obj->argv = argv;

  rc = pthread_create( &thread, NULL, (void *) createSessionAcceptorThread, (void* )test_obj);

  if( rc != 0 ){
     printf("pthread_create failed \n");
     return 0;
  }

  pthread_join(thread, NULL);
  return 0;
}
