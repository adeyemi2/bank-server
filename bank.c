#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bank.h"

BankPtr CreateBank(char* name){
	if(name == NULL){
		printf("NULL name passed to BankCreate");
		return NULL;
	}
	else{
		BankPtr bank;
		bank = (BankPtr) malloc( sizeof( struct Bank));
		if(bank == NULL){
			printf("Malloc failed for CreateBank at bank");
			return NULL;
		}
		pthread_t *threads = malloc(sizeof(pthread_t)*20);
		if(threads == NULL){
			printf("Malloc failed for CreateBank at bank");
			return NULL;
		}
		ActPtr *accounts = malloc(sizeof(struct Account)*20);
		if(threads == NULL){
			printf("Malloc failed for CreateBank at bank");
			return NULL;
		}
		bank->name = name;
		bank->account_index = 0;
		return bank;
	}
	
}

int AddAccount(BankPtr bank, char* name){

	if(bank == NULL){
		printf("Invalid bank?");
		return 0;
	}
	if(bank->account_index > 20){
		printf("Can't add any more accounts to this bank");
		return 0;
	}
	if(name == NULL){
		printf("Can't create an account with no name");
		return 0;
	}
	ActPtr acct;
	acct = AccountCreate(name, bank->account_index);
	if(acct == NULL){
		printf("Failed to create account at index %i", bank->account_index);
		return 0;
	}

	bank->account_index += 1;
	return 1;
}

int AddThread(BankPtr bank, pthread_t thread){
	return 0;
}

void PrintAccounts(BankPtr bank){
	if(bank == NULL){
		printf("Can't print details for NULL bank...");
		return;
	}

	if(bank->accounts == NULL){
		printf("Bank has no account details to print.");
		return;
	}

	//for loop through the bank accounts, how do i do this with just a pointer again???
	//because this doesnt work
	int i = 0;
	while(i<=len(bank->accounts)){
		if(bank->accounts[i] == NULL){

		}
		else{
			PrintAccount(bank->accounts[i]);
			i++;
		}
	}

}

void *threadFunc(void *arg){
	char *str;
	int i = 0;

	str=(char*)arg;

	while(i < 10 )
	{
		sleep(1);
		printf("threadFunc says: %s\n",str);
		++i;
	}

	return NULL;
}


int main( int argc, char** argv ){
	pthread_t pth;	// this is our thread identifier
		int i = 0;

		/* Create worker thread */
		pthread_create(&pth,NULL,threadFunc,"processing...");

		/* wait for our thread to finish before continuing */
		pthread_join(pth, NULL /* void ** return value could go here */);

		while(i < 10 )
		{
			usleep(1);
			printf("main() is running...\n");
			++i;
		}

		return 0;
}

//2 threads when you start main:
//a to lock b and print account details every 20 seconds then return control to b
//b accepts connections from accounts(client)