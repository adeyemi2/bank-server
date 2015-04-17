#include "accounts.h"
#include <string.h>
#include <stdlib.h>



AccountPtr get_account(char *accountname, AccountStoragePtr collection) {
  int i;
  for(i=0; i < collection->accounts.length(); i++){
    if(strcmp(collection->accounts[i]->name, account_name) == 0){
      return collection->accounts[i];
    }
  }

  return NULL;
}

AccountPtr accountCreate(char* name, int index){
	//check account name is valid 
	if(name == NULL){
		printf("NULL name passed to AccountCreate");
		return NULL;
	}
	else{
		AccountPtr acc;
		acc = (AccountPtr) malloc( sizeof( struct Account));
		if(acc == NULL){
			printf("Malloc failed in AccountCreate");
			return NULL;
		}
		acc->name = name;
		acc->balance = 0.0;
		acc->active = 0;
		acc->index = index;
		return acc;
	}
}
//return balance on success, -1 on failure
float getBalance(AccountPtr account){
	if(account == NULL){
		printf("Invalid account");
		return -1.0
	}
	return account->balance;
}
//return 1 for success, 0 for failure
int deposit(float amount, AccountPtr account){
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
int withdraw(float amount, AccountPtr account){
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

void printAccount(AccountPtr account ) {
if(account == NULL){
    printf("No details for NULL account to print");
    return;
  }
  else{
    char * active = "True";
    if (account->active == 0){
      active = "False";
    }
    printf("<Account: %s, Balance: %f, Currently Active: %s >" , account->name, account->balance, active);
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
			printAccount(bank->accounts[i]);
		}
		i++;
	}
}

//return 1 for success, 0 for failure
int addAccount(BankPtr bank, AccountPtr account){
	int i = 0;
	if(bank == NULL){
		printf("Can't print details for NULL bank...");
		return;
	}

	//for loop through the bank accounts, how do i do this with just a pointer again???
	//because this doesnt work
	while(i<=19){
		if(bank->accounts[i] == NULL){
			bank->accounts[i] = account;
			return 1;
		}
		i++;
	}
	return 0;
}
