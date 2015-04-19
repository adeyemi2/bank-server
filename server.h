#ifndef SERVER_H_
#define SERVER_H_

struct user_args{
   int argc;
   char **argv;
};
typedef struct user_args* UserArgs;


struct sock_info{
  int sockfd;
};
typedef struct sock_info* SockInfo;

struct client_request{
  char* command;
  char* argument;
};
typedef struct client_request* ClientRequestPtr;

struct client_response{
  float balance;
  int is_query;
  int command_performed;
  char message[255];
};
typedef struct client_response* ClientResponsePtr;

void error(const char *msg)
{
   perror(msg);
   exit(1);
}


#endif
