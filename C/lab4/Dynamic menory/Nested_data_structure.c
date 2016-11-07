#include <stdio.h>
#include <stdlib.h>

/*
记住free的顺序,从内到外，否则外层free，无法access内层
*/

int main() {

    // this allocates space for the 2 pointers
    // Note:::::: not two int but int* !!!!!!!!
    int **pointers = malloc(sizeof(int *) * 2);
    // the first pointer points to a single integer
    pointers[0] = malloc(sizeof(int));
    // the second pointer pointes to an array of 3 integers
    pointers[1] = malloc(sizeof(int) * 3);

    // let's set their values
    // *ointers[0][0] = 55 also works
    *pointers[0] = 55;

    pointers[1][0] = 100;
    pointers[1][1] = 200;
    pointers[1][2] = 300;


    // do other stuff with this memory

    // now time to free the memory as we are finished with the data-structure
    // first we need to free the inner pieces
    free(pointers[0]);
    free(pointers[1]);
    // now we can free the space to hold the array of pointers themselves
    free(pointers);

    return 0;
}
