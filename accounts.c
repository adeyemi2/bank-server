#include "accounts.h"
#include <string.h>
#include <stdlib.h>


AccountPtr get_account(char *account_name, AccountStorage collection)
{
  int i;
  for(i=0; i < collection->accounts.length(); i++){
    if(strcmp(collection->accounts[i]->name, account_name) == 0){
      return collection->accounts[i];
    }
  }

  return NULL;
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
