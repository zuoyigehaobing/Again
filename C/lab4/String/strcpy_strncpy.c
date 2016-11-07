#include <stdio.h>
#include <string.h>

int main() {
    char s1[5] = "wozh";
    char s2[32] = "University of";
    char s3[3] = "ni";

    // when you strcpy s3 to s1, then s1 would be empty first then start copy!
    strcpy(s1,s3);
    printf("%s\n", s1);
    printf("This is my length of new s1: %lu \n",strlen(s1));




    // This is unsafe because s1 may not have enough space
    // to hold all the characters copied from s2. some times cause errors
    // strcpy(s1, s2);

    // strncpy is a safe function if we use sizeof(s1) and then put a
    // null-terminate at the end
    // This doesn't necessarily null-terminate s1 if there isn't space.
    strncpy(s1, s2, sizeof(s1));
    // So we explicitly terminate s1 by setting a null-terminator.
    // other wise the output should be Unive, not we want
    s1[4] = '\0';


    printf("%s\n", s1);
    printf("%s\n", s2);
    return 0;
}
