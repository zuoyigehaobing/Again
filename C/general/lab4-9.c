#include <stdlib.h>
#include <stdio.h>

/*****
 * Define a function void fib(...) below. This function generates the first
 * n values in the Fibonacci sequence (n is a parameter). Recall that this sequence
 * is defined:     0, 1, 1, 2, 3, 5, ... , fib[n] = fib[n-2] + fib[n-1], ...
 * The values should be stored in a dynamically allocated array composed of
 * exactly the correct number of integers. The values should be returned through
 * a pointer parameter passed in as the first argument.
 *
 * See below for an example call to fib.
 */



void fib(int **ptr,int size){
    static int trial = 0;
    trial ++;
    printf("This is my trial number of static: %d\n",trial);
    *ptr = malloc(sizeof(int) * size);
    if(size == 1){
        ptr[0][0] = 0;}
    if(size == 2){
        ptr[0][1] = 1;}
    if(size >2){
    	ptr[0][0] = 0;
    	ptr[0][1] = 1;
   		 for(int i=2;i<size;i++){
		ptr[0][i] =   ptr[0][i-2] + ptr[0][i-1];

    	}
    }

}


int main(int argc, char **argv) {
    int count = strtol(argv[1], NULL, 10);
    int *fib_sequence;
    fib(&fib_sequence, count);
    fib(&fib_sequence, count);
    fib(&fib_sequence, count);
    fib(&fib_sequence, count);
    for(int i = 0;i<count;i++){
      printf("%d \n", fib_sequence[i]);
    }
    return 0;
}
