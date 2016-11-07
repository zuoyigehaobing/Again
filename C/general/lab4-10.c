#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Return a pointer to an array of two strings. The first is the characters
   of string s that are at even indices and the second is the characters from
   s that are at odd indices */
char **parity_strings(const char *s) {
    char **result = malloc(sizeof(char*)*2);
    int half = strlen(s) / 2 + 2;
    result[0] = malloc( sizeof(char)*half);
    result[1] = malloc( sizeof(char)*half);
    int e=0;
    int o=0;
    for(int i=0;i<strlen(s);i++){
        if(i%2 == 0){
            result[0][e] = s[i];
            e = e+1;
        }
        else{
            result[1][o] = s[i];
            o = o + 1;
        }


    }
    return result;

}

int main(int argc, char **argv) {
    char **r = parity_strings(argv[1]);
    printf("%s %s %s", r[0], r[1], argv[1]);
    return 0;
}
