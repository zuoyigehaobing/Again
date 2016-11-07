#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function prototype:
char ** combine_names(char *firstnames, char *lastnames);

int main(int argc, char **argv) {
    char *firstnames, *lastnames;

    firstnames = "Shanbbbzz,shan,s";

    lastnames = "bingzzzzzhao,bingzhao,bz";
    // Calling combine_names():
    char **fullnames = combine_names(firstnames, lastnames);
    //combine_names(firstnames, lastnames);
    printf("This is my first elements in the names: %s\n",fullnames[0]);
    /* hidden test code */

    return 0;
}
// Write your function here.
char ** combine_names(char  * firstnames, char* lastnames){

    const char seg[2] = ",";
    int len = strlen(firstnames) + strlen(lastnames);
    char ** result = malloc(sizeof(char*)*len);
    for(int m = 0; m < len;m++){
      result[m] = malloc(sizeof(char)*len);
    }
    char firstname[len];
    char lastname[len];

    strcpy(firstname,firstnames);
    strcpy(lastname,lastnames);

    char *first;
    char *last;

    int inx = 0;
    char *space = " ";


    first = strtok(firstname,seg);
    while(first!= NULL){
        strcpy(result[inx],first);
        first = strtok(NULL,seg);
        inx ++;
    }

    int ins =  0;
    while(ins < inx){
      strcat(result[ins],space);
      ins ++;
    }

    last = strtok(lastname,seg);
    int iny = 0;
    while(last!= NULL){
        strcat(result[iny],last);
        last = strtok(NULL,seg);
        iny ++;
    }

    for(int k = 0;k<inx;k++){
      printf("This is my %d element in result : %s\n",k,result[k]);
    }
    return result;
}
