#include "qtree.h"

int main (int argc, char ** argv) {
    QNode *root = NULL;
	Node *q_list;
    
    if (argc < 3) {
        printf ("To run the program ./test2 <name of input file> <user name> <answer(s)>\n");
        return 1;
    }
    
    q_list = get_list_from_file (argv[1]);
    
    if (q_list == NULL) {
        printf ("The list is empty\n");
        return 1;
    }
    
    int qcount = 0;
    // iterate through the questions list to find the number of questions
    for (Node *q = q_list; q; q = q->next, qcount++);
    // check that the argument count matches the question count
    if (qcount != argc-3){
        fprintf(stderr, "ERROR: Incorrect number of answers\n");
        return 1;
    }
    
    // create tree
    root = add_next_level (root, q_list);
    
    free_list (q_list);   
    
    // add user to tree
    
    // find the list of friends to which the user should be attached
    QNode *child = root;
    for (int ans = 3; ans < argc-1; ans++){
        child = find_branch(child, atoi(argv[ans]));
    }
    
    // add user to the end of the list
    child->children[atoi(argv[argc-1])].fchild = 
        add_user(child->children[atoi(argv[argc-1])].fchild, argv[2]);
        
    print_qtree(root, 0);
    
    free_qtree(root);

    return 0;
}
