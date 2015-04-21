#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CONNECT_STATEMENT "Trying to connect again in 3 seconds..."
#define QUIT_STATEMENT "quit\0"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void signal_handler(int signo)
{
    if(signo == SIGINT){
        printf("Signal Interrupt handled. Exiting \n");
        exit(1);
    }
}

void server_listener(void * params)
{
  char buffer[255];
  int socket;

  socket = (int) params;
  while( read(socket, buffer, 255) > 0){

    // handle the quit from the server
    if(strcmp("quit", buffer) == 0){
      signal_handler(SIGINT);
    };
    // output the response from the server
    if( strlen(buffer) > 0) {
      printf("%s\n", buffer);
    }
    //printf(">>");
    bzero(buffer,255);
  }

}

void client_listener(void * params)
{
  char client_buffer[255];
  int socket, n;
  socket = (int) params;

  while( fgets(client_buffer, 255, stdin) != NULL ) {
      // send input to server
      n = send(socket, client_buffer, strlen(client_buffer), MSG_OOB);
      if(n < 0)
        error("ERROR writing to socket");
      printf(">>");
  }
}

int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t server_listener_thread, client_listener_thread;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    // create a thread for listening to the server
    else
      pthread_create( &server_listener_thread, NULL,
       (void*(*)(void*))server_listener, (void*)sockfd);

    pthread_create( &client_listener_thread, NULL,
      (void*(*)(void*))client_listener, (void*)sockfd);

    pthread_join(server_listener_thread,NULL);
    pthread_join(client_listener_thread,NULL);
}
