//
//  main.c
//  A1
//
//  Created by Bingzhao Shan on 2016-10-20.
//  Copyright © 2016 Bingzhao Shan. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function prototype:
char **combine_names(char *firstnames, char *lastnames);

int main(int argc, char **argv) {
    char trial[80] = "this is my strtok trial";
    const char s[2] = " ";
    char * token;
    
    token = strtok(trial,s);
    printf("This is my first destination : ");
    
    return 0;
}
