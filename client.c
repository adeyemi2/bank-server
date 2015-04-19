#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char server_buffer[256], client_buffer[256];


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
    else
      read(sockfd,server_buffer,255);

    printf("%s >>", server_buffer);
    bzero(server_buffer,255);
    while( fgets(client_buffer, 255, stdin) != NULL ) {
        // send input to server
        n = send(sockfd, client_buffer, strlen(client_buffer), MSG_OOB);
        if(n < 0)
          error("ERROR writing to socket");

        // wait for response
        n = read(sockfd,server_buffer,255);

        printf("%i \n", strcmp("quit", server_buffer) == 0);
        if(strcmp("quit", server_buffer) == 0){
            printf("SIGINT: %i \n", server_buffer[0]);
            signal_handler(SIGINT);
        }

        if (n < 0)
          error("ERROR reading from socket");

        printf("%s \n", server_buffer);
        bzero(server_buffer,255);
        printf(">> ");
    }

    close(sockfd);
    return 0;
}
