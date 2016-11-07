#include <stdio.h>
#include <stdlib.h>
/*
   function make a copy of any inputs inside the function.Thus the following
   code doesn't work; But the pointer relationship between different memoring
   area remains all the time.
   因为data的数值在function内部被换成了malloc的地址。但是这是data的备份不是原函数重的data

   注意一个helper function无法改变一个东西的具体数值是什么，只能改变它的地址指向的东西，
   所以试图直接改变它的数值应该给helper function更低一层的指针

   比如，试图改变 int i
   需要给 &i

   试图改变 *data
   需要给 &data

   void helper(int *data) {
      // let's make an array of 3 integers on the heap
      printf("My memory 1: %lu \n",&data);

      // 在function内部定义或者改变的变量data不再是input data，而是一个新的变量
      data = malloc(sizeof(int) * 3);
      printf("My memory 2: %lu \n",&data);
      data = malloc(sizeof(int) * 3);
      printf("My memory 3: %lu \n",&data);
      data[0] = 18;
      data[1] = 21;
      data[2] = 23;
   }

   int main() {
       int *data;
       printf("My memory : %lu \n",&data);
       helper(data);

       // let's just access one of them for demonstration
       printf("the middle value: %d\n", data[1]);

       return 0;
   }


*/





void helper(int **arr_matey) {
   // let's make an array of 3 integers on the heap
   *arr_matey = malloc(sizeof(int) * 3);

   int *arr = *arr_matey;
   arr[0] = 18;
   arr[1] = 21;
   arr[2] = 23;
}

int main() {
    int *data;
    helper(&data);

    // let's just access one of them for demonstration
    printf("the middle value: %d\n", data[1]);

    free(data);
    return 0;
}
