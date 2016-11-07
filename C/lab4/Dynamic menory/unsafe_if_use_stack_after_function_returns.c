// unsafe behaviour will happen if you use the address on the
// stack after thre function returns
#include <stdio.h>

int * seti(){
  int i = 5;
  return &i;
}

int junk(){
  int junk = 999;
  return junk;
}

int main(){
  int *i = seti();
  printf("This is my i : %d\n",*i);
  junk();
  printf("This is my i after junk : %d\n",*i);
  return 0;
}
