#include <stdio.h>
#include <string.h>

int main() {
    char s1[30] = "University of C Programming";
    char *p;

    printf("This is the start of s1 : %c \n",*s1);
    printf("This is the second element of s1 : %c \n",*(s1+1));

    // find the index of the first 'v'
    p = strchr(s1, 'v');
    if (p == NULL) {
        printf("Character not found\n");
    } else {
        // you can do pointer subtraction since the memory of s1 is continuous
        // and the size of char is only 1.
        printf("Character found at index %ld\n", p - s1);
    }

    int i[20] = {1,6,3,4,6,7,8,9,0};
    // when you add one, you automatically add 1 times sizeof(int)

    printf("This is the start of i : %d \n",*(i+1));
    printf("address of i[0] : %ld \n",&i[0]);
    printf("address of i[1] : %ld \n",&i[1]);
    printf("subtraction : %ld \n",&i[1] - &i[0]);

    // find the first token (up to the first space)
    p = strchr(s1, ' ');
    if (p != NULL) {
        *p = '\0';
    }
    printf("%s\n", s1);

    return 0;
}
