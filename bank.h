#ifndef BANK_H
#define BANK_H
#include <stdlib.h>
#include <pthread.h>
#include "account.h"

struct Bank{
	char* name;
	pthread_t * threads[20];
	ActPtr * accounts[20];
	int account_index;
};

typedef struct Bank* BankPtr;

BankPtr CreateBank(char* name);

int AddAccount(BankPtr bank, char* name);

int AddThread(BankPtr bank, pthread_t thread);

void PrintAccounts(BankPtr bank);

void *threadFunc(void *arg);

#endif
