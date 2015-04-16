#include <stdio.h>
#include <stdlib.h>
#include "account.h"

//-account create
AccountPtr AccountCreate(char* name, int index){
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

//-account query
float getBalance(AccountPtr account){
	if(account == NULL){
		printf("Invalid account");
	}
	return account->balance;
}

//-account deposit
//deposit should change account balance and return 0 for failure 1 for success
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

//withdraw should change account balance and return 0 for failure 1 for success
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

//-account serve
//AccountServe should start a service session with an account and set account.active to 1
void AccountServe(AccountPtr account){
	return;
}

//CloseSession should close a service session with an account if one is active and set account.active to 0
void CloseSession(AccountPtr account){
	return;
}

void PrintAccount(AccountPtr account){
	if(account == NULL){
		printf("No details for NULL account to print");
		return;
	}
	else{
		char * active = "True";
		if (account->active == 0){
			active = "False";
		}
		printf("%s\nBalance:\t%f\nCurrently Active:\t%s" , account->name, account->balance, active);
	}
}
