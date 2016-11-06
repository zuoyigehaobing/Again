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
    // for test : if given filename,username and answer:


    printf("\n");
    if(argc == 4){
	add_user_to_leaves(argv[2],argv[3],root);
	printf("============ the result of my own tests : \n");
	printf("take username : %s ; take answer : %s\n",argv[2],argv[3]);
	printf("\n");
	print_qtree(root,0);
	Node * head = search(argv[2],root);
	if(head == NULL)
		printf("Search functionality failed\n");
    }


    free_tree(root);
    free_list(q_list);


    return 0;
}
