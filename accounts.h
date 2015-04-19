#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_
#define MAX_ACCOUNTS 20


struct account{
  char *name;
  float balance;
  int in_session;
  int index;
};
typedef struct account* AccountPtr;

struct account_storage{
  int threads[MAX_ACCOUNTS];
  AccountPtr accounts[MAX_ACCOUNTS];
};
typedef struct account_storage* AccountStoragePtr;

AccountPtr accountGet(char *accountname, AccountStoragePtr collection);

//-account create
AccountPtr accountCreate(char* name, AccountStoragePtr all_accounts);

//-account query
float accountGetBalance(AccountPtr account);

//-account deposit
//deposit should change account balance and return 0 for failure 1 for success
int accountDeposit(float amount, AccountPtr account);

//withdraw should change account balance and return 0 for failure 1 for success
int accountWithdraw(float amount, AccountPtr account);

// print account details
void accountPrint(AccountPtr account);

void printAccounts(AccountStoragePtr bank);

void accountServe(int thread, char* account_name, AccountStoragePtr all_accounts);

void accountEndConnection(int thread, AccountStoragePtr all_accounts);

AccountPtr getThreadAccount(int thread, AccountStoragePtr all_accounts);
#endif
