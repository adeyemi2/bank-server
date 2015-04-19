#include <stdio.h>
#include <stdlib.h>
#include "accounts.h"

int main(void)
{
   AccountStoragePtr storage; 
   storage = malloc(sizeof(struct account_storage));

   /* Test accountCreate */
   AccountPtr test;
   test = accountCreate("shreyas", 0);

   return 0;
}
