#include "qtree.h"

int main (int argc, char ** argv) {
    QNode *root = NULL;
	Node *q_list;
    
    if (argc < 2) {
        printf ("To run the program ./test2 <name of input file>\n");
        return 1;
    }
    
    q_list = get_list_from_file (argv[1]);
    
    if (q_list == NULL) {
        printf ("The list is empty\n");
        return 1;
    }
    
    //that is only the first test to test creation of the question tree
    root = add_next_level (root, q_list);
	print_qtree (root, 0);
    
    
    
    
    
    
    //your tests here
    
    char  answer[4] = "0101";
    
    add_user_to_leaves("shan",answer,root);
    add_user_to_leaves("NI BABA",answer,root);
    
    
    print_qtree (root, 0);
    
    Node *head = search("NI BABA",root);
    if(head == NULL){
        printf("LOL\n");
    }
    print_list(head);
    
    return 0;
}
