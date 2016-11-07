// out of memory erroe : ENOMEM
#include <stdio.h>
#include <stdlib.h>

int play_with_memory() {
    int i;
    int *pt = malloc(sizeof(int));

    i = 15;
    *pt = 49;

    // What happens if you comment out this call to free?
    free(pt);

    // Shan ; you can still dereference pt after it's freed
    //        free simply tell the system that these memory
    //        is now available for use but doesn't reset the
    //        value itself. But please don't do it since it's unsafe. 
    printf("%d\n", *pt);
    //*pt = 7;

    //printf("%d\n", *pt);

    return 0;
}

int main() {
    play_with_memory();
    play_with_memory();
    play_with_memory();
    return 0;
}
