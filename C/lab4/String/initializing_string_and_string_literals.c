/*
    Ways to initialize a string variable
*/
# include<stdio.h>


int main(){
  // first way to daclare a string
  char text0[20] = {'h','e','l','l','o','\0'};
  printf("My result 1: %s \n",text0);

  // second way: don't forget that null terminator "\0" is also a character
  // par attention to the space you decalre
  // in this case: the first five chars in the array are 'h' 'e' 'l' 'l' 'o'
  // the rest 15 are all '\0'
  char text1[20] = "hello";
  printf("My result 2: %s \n",text1);

  // The third way: the compiler will allocate the number of chars + 1 size of space
  // the following is the same as : text2[6] = "hello";
  // once the string is created, it is fixed size
  char text2[] = "hello";
  printf("My result 2: %s \n",text2);



  // you can change any char after you decalre the string in above ways:
  char text3[20] = "hello";
  text3[0] = 'j';
  printf("My result 3: %s \n",text3);

  // however, if you declare a string literal : which is string constants(you
  // decalre a pointer points to some string constants), you
  // can't change any of them
  char * text4 = "hello";
  // text4[0] = 'j';   if you do this, you get a bus error
  printf("My result 4: %s \n",text4);


  return 0;
}
