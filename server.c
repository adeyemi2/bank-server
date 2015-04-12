#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct user_args{
   int argc;
   char **argv;
};
typedef struct user_args* UserArgs;

void error(const char *msg)
{
   perror(msg);
   exit(1);
}


int session_acceptor(void* params)
{
   UserArgs args = (UserArgs)params;
   printf("Session Acceptor Thread Created! \n");
   printf("argc : %d \t", args->argc);
   printf("argv: %s \n", args->argv[1]);

   //TCP/IP server logic
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n;
   
   if( args->argc < 2 ){
      fprintf(stderr, "ERROR, no port provided \n");
      pthread_exit(NULL);
   }  

   sockfd = socket( AF_INET, SOCK_STREAM, 0 );
   if( sockfd < 0 ){
      error("ERROR opening socket");
   }

   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(args->argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons( portno );
   if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ){
      error("ERROR on binding");
   }

   listen(sockfd, 5);
   clilen = sizeof(cli_addr);
   newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
   if( newsockfd < 0 ){
      error("ERROR on accept");
   }

   bzero(buffer, 256);
   n = read( newsockfd, buffer, 255 );
   if( n < 0 ){
      error("ERROR reading from socket \n");
   }

   printf("Here is the message: %s \n", buffer);
   n = write(newsockfd, "I got your message", 18);
   if( n < 0 ){
      error("ERROR writing to socket");
   }

   close(newsockfd);
   close(sockfd);
   pthread_exit(NULL);
   return 0;
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

  rc = pthread_create( &thread, NULL, (void *) session_acceptor, (void* )test_obj);
  
  if( rc != 0 ){
     printf("pthread_create failed \n");
     return 0;
  }

  pthread_join(thread, NULL);
  return 0;
}
