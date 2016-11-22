#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "questions.h"

Node * get_list_from_file (char *input_file_name) {
	Node * head = NULL;
	
    //read lines and add to the list
    
    //STEP 1; declare a pointer to the file and open it.
    FILE * my_file;
    my_file = fopen(input_file_name,"r");
    if(my_file == NULL){
        fprintf(stderr,"Error opening file \n");
    }
    
    //STEP 2: loop over the file line by line
    char line[200];
    Node *curr = NULL;
    Node * new = NULL;
    while( fgets(line,200,my_file) != NULL){
        line[strcspn(line,"\r\n")] = '\0';
        if(strlen(line) == 0){
            continue;
        }
        else if(curr == NULL){
            head = one_node(line);
            curr = head;
        }
        else{
            new = one_node(line);
            curr->next = new;
            curr = curr->next;
        }
    }
    curr->next = NULL;
	return head;
}

void print_list (Node *head) {
    
    Node *curr = head;
    while(curr != NULL){
        printf("%s\n",curr->str);
        curr = curr->next;
    }
}

void free_list (Node *head) {
    Node * curr = head;
    Node * old = NULL;
    while(curr != NULL){
        old = curr;
        curr = curr->next;
        free(old->str);
        free(old);
    }
}


Node * one_node(char*line){
    Node * n = (Node*) calloc(1,sizeof(Node));
    n->str =(char*) calloc(1,sizeof(char)*(strlen(line)+1));
    strcpy(n->str,line);
    return n;
}




