# include<stdio.h>
# include <string.h>

int main(){
  char Monday[20] = "Monday";
  // size of Monday should be 20 since compiler doesn't care what's in the array

  printf("This is my size of Monday: %lu \n",sizeof(Monday));
  printf("This is my length of Monday %lu \n",strlen(Monday));
  return 0;
}
