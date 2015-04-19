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

void accountServe(int thread, char* account_name, AccountStoragePtr all_accounts){
	/* Find account in accounts that matches name of account name */
	int tmp_account_index;
	tmp_account_index = 0;
	do{
		if(strcmp(all_accounts->accounts[tmp_account_index]->name, account_name) == 0)
			break;
    tmp_account_index += 1;
	} while( strcmp(all_accounts->accounts[tmp_account_index]->name, account_name) != 0);

	all_accounts->accounts[tmp_account_index]->in_session = 1;
	all_accounts->threads[tmp_account_index] = thread;
}

void accountEndConnection(int thread, AccountStoragePtr all_accounts) {
  int thread_index;
  thread_index = 0;
  /* find the index that corresponds to the account and socket */
  while(thread_index < MAX_ACCOUNTS) {
    if( all_accounts->threads[thread_index] != 0 && all_accounts->threads[thread_index] == thread) {
      break;
    }
    thread_index++;
  }

  all_accounts->accounts[thread_index]->in_session = 0;
  all_accounts->threads[thread_index] = 0;

}

AccountPtr accountCreate(char* name, AccountStoragePtr all_accounts){
	int i;

  //check account name is valid
	if(name == NULL || strcmp(name, "") == 0){
		printf("NULL name passed to AccountCreate");
		return NULL;
	}
  // check if name exists
  i = 0;
	while( i < MAX_ACCOUNTS) {
    if( all_accounts->accounts[i] == NULL ) {
      break;
    }
    if( strcmp(all_accounts->accounts[i]->name, name) == 0) {
      printf("Account name already exists\n");
      return NULL;
    }
    i++;
  }

	AccountPtr acc;
	acc = (AccountPtr) malloc( sizeof( struct account));
	if(acc == NULL){
		printf("Malloc failed in AccountCreate");
		return NULL;
	}
	acc->name = name;
	acc->balance = 0.0;
	acc->in_session = 0;
	//acc->index = index;
	return acc;

}

//return balance on success, -1 on failure
float accountGetBalance(AccountPtr account){
	if(account == NULL){
		printf("Invalid account");
		return -1.0;
	}
	return account->balance;
}
//return 1 for success, 0 for failure
int accountDeposit(float amount, AccountPtr account){
	if(account == NULL){
		printf("Invalid account");
		return 0;
	}
	if(amount < 0){
		printf("Invalid amount, use withdraw to withdraw");
		return 0;
	}

	float deposit_amount, previous_balance, new_balance;
	deposit_amount = (float)amount;
	previous_balance = (float)account->balance;
	new_balance = deposit_amount + previous_balance;
	account->balance = new_balance;
	return 1;
}
//return 1 for success, 0 for failure
int accountWithdraw(float amount, AccountPtr account){
	if(account == NULL){
		printf("Invalid account");
		return 0;
	}
	if(amount < 0){
		printf("Invalid amount, withdraw positive ammounts");
		return 0;
	}

	if(amount > account->balance){
		printf("Can't overdraft at this bank, sorry");
		return 0;
	}
	float withdraw_amount, previous_balance, new_balance;
	withdraw_amount = (float) amount;
	previous_balance = (float) account->balance;
	new_balance = previous_balance - withdraw_amount;
	account->balance = new_balance;
	return 1;
}

void accountPrint(AccountPtr account ) {
  if(account == NULL){
    printf("No details for NULL account to print");
    return;
  }
  else{
    char * active = "True";
    if (account->in_session == 0){
      active = "False";
    }
    printf("<Account: %s, Balance: %f, Currently Active: %s >\n" , account->name, account->balance, active);
  }
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
  for(i; i< MAX_ACCOUNTS; i++) {
    if( all_accounts->threads[i] == thread)
      acct = all_accounts->accounts[i];
      break;
  }

  if( acct != NULL ) {
    return acct;
  }

  return NULL;

}
