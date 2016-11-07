#include <stdio.h>
#include <stdlib.h>

void *squares(int size){
  int* result = malloc(sizeof(int) * size);
  for(int i = 0; i<size;i++){
    result [i] = i * i;
  }

  return result;
}

int main(){
  int *squares_10 = squares(10);
  int i;
  for (i = 0; i < 10;i++){
    printf("Here is my current i: %d \n",squares_10[i]);
  }
  return 0;
}
