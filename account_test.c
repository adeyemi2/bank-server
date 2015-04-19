#include <stdio.h>
#include "accounts.h"


int main(int argc, char** argv){

  AccountPtr acc;
  acc = accountCreate("will", 0);
  accountDeposit(100000.0, acc);
  accountWithdraw(100, acc);
  accountDeposit(100000.0, acc);
  accountDeposit(100000.0, acc);
  accountDeposit(100000.0, acc);
  printAccount(acc);
  printf("%f\n", accountGetBalance(acc));
}
