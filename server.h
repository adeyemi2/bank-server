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

void error(const char *msg)
{
   perror(msg);
   exit(1);
}


#endif