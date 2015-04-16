#include "accounts.h"
#include <string.h>
#include <stdlib.h>


Account get_account(char *accountname, AccountStorage collection)
{
  int i;
  for(i=0; i < collection->accounts.length(); i++){
    if(strcmp(collection->accounts[i]->name, accountname) == 0){
      return collection->accounts[i];
    }
  }

  return NULL;
}