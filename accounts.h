#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_
#define MAX_ACCOUNTS 20


struct account{
  char *name;
  float balance;
  int in_session;
};
typedef struct account* AccountPtr;

struct account_storage{
  int threads[MAX_ACCOUNTS];
  AccountPtr accounts[MAX_ACCOUNTS];
};
typedef struct account_storage* AccountStoragePtr;

AccountPtr get_account(char *accountname);

//-account create
AccountPtr accountCreate(char* name, int index);

//-account query
float getBalance(AccountPtr account);

//-account deposit
//deposit should change account balance and return 0 for failure 1 for success
int deposit(float amount, AccountPtr account);

//withdraw should change account balance and return 0 for failure 1 for success
int withdraw(float amount, AccountPtr account);

// print account details
void PrintAccount(AccountPtr account);

void accountServe(int thread, char* account_name, AccountStoragePtr all_accounts);

void accountEndConnection(int thread, AccountStoragePtr all_accounts);

#endif
