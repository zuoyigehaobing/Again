#include <stdio.h>
#include <string.h>

int strip_q_marks(char *sentence);

int main(int argc, char **argv) {
    int result = strip_q_marks(argv[1]);
    printf("%s %d", argv[1], result);
    return 0;
}

int strip_q_marks(char *sentence){
    int len;
    int result = 0;
    // make sure you pass the address of the string instrad of the 
    // string itself to the strlen function
    for (len = strlen(*sentence); len>0 ;len--){
        if(sentence[len] == '?' && sentence[len+1] == '\0'){
            result ++;
            sentence[len] = '\0';
        }
    }
    return result;
}


