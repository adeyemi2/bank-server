#include "accounts.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

AccountPtr accountGet(char *account_name, AccountStoragePtr collection) {
  int i;
  for(i=0; i < MAX_ACCOUNTS; i++){
    if(strcmp(collection->accounts[i]->name, account_name) == 0){
      return collection->accounts[i];
    }
  }

  return NULL;
}


//return balance on success, -1 on failure
float accountGetBalance(int thread, AccountStoragePtr all_accounts){
	int thread_index;
	thread_index = 0;
	while(thread_index < MAX_ACCOUNTS ){
		if(all_accounts->threads[thread_index] == thread){
			break;
		}
		thread_index++;
	}
	if(thread_index == MAX_ACCOUNTS){
		printf("Thread could not be found \n");
		return -1;
	}
	return all_accounts->accounts[thread_index]->balance;
}


//return 1 for success, 0 for failure
int accountWithdraw(int thread, float amount, AccountStoragePtr all_accounts){
	/* Figure out the account from the thread */
	int thread_index;
	thread_index = 0;
	while(thread_index < MAX_ACCOUNTS ){
		if(all_accounts->threads[thread_index] == thread){
			break;
		}
		thread_index++;
	}
	if(thread_index == MAX_ACCOUNTS){
		printf("Thread could not be found \n");
		return 0;
	}

	if(all_accounts->accounts[thread_index]->balance - amount < (float) 0){
		printf("Balance would be negative. \n");
		return 0;
	}
	all_accounts->accounts[thread_index]->balance -= amount;
	return 1;

}

//return 1 for success, 0 for failure
int accountDeposit(int thread, float amount, AccountStoragePtr all_accounts){
		/* Figure out the account from the thread */
		int thread_index;
		thread_index = 0;
		while( thread_index < MAX_ACCOUNTS ){
			if( all_accounts->threads[thread_index] == thread){
				break;
			}
			thread_index++;
		}
		if(thread_index == MAX_ACCOUNTS){
			printf("Thread could not be found. \n");
			return 0;
		}
		/* Thread index is the same as account index */
		all_accounts->accounts[thread_index]->balance += amount;
		return 1;
}


int accountServe(int thread, char* account_name, AccountStoragePtr all_accounts){
	/* Find account in accounts that matches name of account name */
	int tmp_account_index;
	tmp_account_index = 0;
  // end any account connection that already exists on the thread
  accountEndConnection(thread, all_accounts);
  while(tmp_account_index < MAX_ACCOUNTS ) {
		if( all_accounts->accounts[tmp_account_index] != NULL &&
      strcmp(all_accounts->accounts[tmp_account_index]->name, account_name) == 0) {
        all_accounts->accounts[tmp_account_index]->in_session = 1;
        all_accounts->threads[tmp_account_index] = thread;
        return 1;
    }
    tmp_account_index++;
	}
  printf("Account '%s' not found\n", account_name);
  return 0;
}

void accountEndConnection(int thread, AccountStoragePtr all_accounts) {
  int thread_index;
  thread_index = 0;
  /* find the index that corresponds to the account and socket */
  while(thread_index < MAX_ACCOUNTS) {
    if( all_accounts->threads[thread_index] == thread) {
      all_accounts->accounts[thread_index]->in_session = 0;
      all_accounts->threads[thread_index] = 0;
      return;
    }
    thread_index++;
  }
}

AccountPtr accountCreate(char* name, AccountStoragePtr all_accounts){
	int i;
  AccountPtr acc;

  //check account name is valid
	if(name == NULL || strcmp(name, "") == 0){
		printf("NULL name passed to AccountCreate");
		return NULL;
	}

  if(all_accounts == NULL ) {
    printf("Account registry is NULL");
    return NULL;
  }

  if( all_accounts->last_account_index == MAX_ACCOUNTS -1) {
    printf("Account registry is full. Cannot add account");
    return NULL;
  }
  // check if name exists
  i = 0;

	while( i <= all_accounts->last_account_index) {
    if( all_accounts->accounts[i] == NULL) {
      break;
    }
    if( strcmp(all_accounts->accounts[i]->name, name) == 0) {
      printf("Account name already exists\n");
      return NULL;
    }
    i++;
  }

	acc = (AccountPtr) malloc( sizeof( struct account));
	if(acc == NULL){
		printf("Malloc failed in AccountCreate");
		return NULL;
	}
  if (all_accounts->accounts[all_accounts->last_account_index] != NULL) {
    all_accounts->last_account_index++; //
  }
  acc->name = (char*)malloc(strlen(name));
  strcpy(acc->name, name);
	acc->balance = 0.0;
	acc->in_session = 0;
	acc->index = all_accounts->last_account_index;
  // add the account to the list of accounts
  all_accounts->accounts[all_accounts->last_account_index] = acc;
	return acc;

}

void destroyAccount(AccountPtr account) {
  if(account != NULL) {
    free(account->name);
    free(account);
  }
}

void accountPrint(AccountPtr account ) {
  if(account == NULL){
    return;
  }

  printf("\t<Account: %s, Balance: %f, Currently Active: %s >\n" , account->name, account->balance, (account->in_session == 1 ? "True" : "False"));

}

void printAccounts(AccountStoragePtr bank){
	int i = 0;
	if(bank == NULL){
		printf("Can't print details for NULL bank...");
		return;
	}

	//for loop through the bank accounts, how do i do this with just a pointer again???
	//because this doesnt work
	while(i <= MAX_ACCOUNTS){
		if(bank->accounts[i] != NULL){
			accountPrint(bank->accounts[i]);
		}
		i++;
	}
}

//return 1 for success, 0 for failure
int addAccount(AccountStoragePtr bank, AccountPtr account){
	int i = 0;
	if(bank == NULL){
		printf("Can't print details for NULL bank...");
		return 0;
	}

	//for loop through the bank accounts, how do i do this with just a pointer again???
	//because this doesnt work
	while(i<=MAX_ACCOUNTS){
		if(bank->accounts[i] == NULL){
			bank->accounts[i] = account;
			return 1;
		}
		i++;
	}
	return 0;
}

AccountPtr getThreadAccount(int thread, AccountStoragePtr all_accounts) {
  AccountPtr acct;
  int i;
  i = 0;
  acct = all_accounts->accounts[i];
  for(;i< MAX_ACCOUNTS; i++) {
    if( all_accounts->threads[i] == thread)
      acct = all_accounts->accounts[i];
      break;
  }

  if( acct != NULL ) {
    return acct;
  }

  return NULL;

}
