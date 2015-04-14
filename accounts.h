#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_
#define MAX_ACCOUNTS 20


struct account{
  char *name;
  float balance;
  int in_session;
}
typedef struct account* Account;

struct account_storage{
  Account[MAX_ACCOUNTS] accounts;
}
typedef struct account_storage* AccountStorage;

Account get_account(char *accountname);


#endif