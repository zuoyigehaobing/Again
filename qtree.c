#include "qtree.h"


QNode *add_next_level (QNode *current, Node *list_node) {
	int str_len;
	
	str_len = strlen (list_node->str);
	current = (QNode *) calloc (1, sizeof(QNode));

	current->question =  (char *) calloc (str_len +1, sizeof(char ));
	strncpy ( current->question, list_node->str, str_len );
	current->question [str_len] = '\0';  
	current->node_type = REGULAR;
	
	if (list_node->next == NULL) {
		current->node_type = LEAF;
		return current;
	}
	
	current->children[0].qchild = add_next_level ( current->children[0].qchild, list_node->next);
	current->children[1].qchild = add_next_level ( current->children[1].qchild, list_node->next);

	return current;
}

void print_qtree (QNode *parent, int level) {
	int i;
	for (i=0; i<level; i++)
		printf("\t");
	
	printf ("%s type:%d\n", parent->question, parent->node_type);
	if(parent->node_type == REGULAR) {
		print_qtree (parent->children[0].qchild, level+1);
		print_qtree (parent->children[1].qchild, level+1);
	}
	else { //leaf node
		for (i=0; i<(level+1); i++)
			printf("\t");
		print_users (parent->children[0].fchild);
		for (i=0; i<(level+1); i++)
			printf("\t");
		print_users (parent->children[1].fchild);
	}
}

void print_users (Node *parent) {
	if (parent == NULL)
		printf("NULL\n");
	else {
		printf("%s, ", parent->str);
		while (parent->next != NULL) {
			parent = parent->next;
			printf("%s, ", parent->str);
		}
		printf ("\n");
	}
}


// still no ideal what this function is used for
// T1: helper function to free the question tree
void traversal(QNode * current){
    
    if(current->node_type == LEAF){
        printf("I am leaf \n");
        return;
    }
    
    traversal(current->children[0].qchild);
    // do something to itself
    traversal(current->children[1].qchild);
    
}







void add_user_to_leaves(char * username,char * answer,QNode *root){
    
    int this_answer = 2;
    int check = answer[0] - '0';
    if(check == 0){
        this_answer = 0;
    }
    if(check == 1){
        this_answer = 1;
    }
    
    
    // LEAF
    if(root->node_type == LEAF){
        if(this_answer == 0){
            // if there is no linked-list create one
            if(root->children[0].fchild == NULL){
                root->children[0].fchild = one_node(username);
                root->children[0].fchild->next = NULL;
            }
            
            // if there is already an linked-list, loop over
            else{
                Node * curr = root->children[0].fchild;
                root->children[0].fchild = one_node(username);
                root->children[0].fchild->next = curr;
                
            }
            
            
        }
        else if(this_answer == 1){
            // if there is no linked-list create one
            if(root->children[1].fchild == NULL){
                root->children[1].fchild = one_node(username);
                root->children[1].fchild->next = NULL;
            }
            
            // if there is already an linked-list, intsert it to head
            else{
                Node * curr = root->children[1].fchild;
                root->children[1].fchild = one_node(username);
                root->children[1].fchild->next = curr;
            }
            
        }
        return;
    }
    

    // REGULAR  :move to next answer and Recursion
    else{
        
        if(this_answer == 0){
            root = root->children[0].qchild;
            answer = answer + 1;
            add_user_to_leaves(username,answer,root);
        }
        else if(this_answer == 1){
            root = root->children[1].qchild;
            answer = answer + 1;
            add_user_to_leaves(username,answer,root);
        }
    }
    
}


Node * search(char * username, QNode * root){
    Node * head = NULL;
    
    // if root is NULL or username is NULL ,return NULL
    if(root == NULL || username == NULL){
        return NULL;
    }
    
    //if QNode and Regular,Recursion
    if(root->node_type == REGULAR){
        head = search(username,root->children[0].qchild);
        if(head != NULL)
            return head;
        head = search(username,root->children[1].qchild);
        if(head != NULL)
            return head;
    }
    
    
    // if QNode and LEAF, go to linked-list
    else if(root->node_type == LEAF){
        //left
        head = root->children[0].fchild;
        Node * curr = head;
        while(curr != NULL){
            if(strcmp(curr->str,username) == 0 ){
                return head;
            }
            curr = curr->next;
        }
        
        // right
        head = root->children[1].fchild;
        curr = head;
        while(curr != NULL){
            if(strcmp(curr->str,username) == 0 ){
                return head;
            }
            curr = curr->next;
        }
        
        return NULL;
        
    }
    
    
    return head;
        
}



void free_tree(QNode * root){
    
    // LEAF
    if(root->node_type == LEAF){
        free_list(root->children[0].fchild);
        free_list(root->children[1].fchild);
        free(root->question);
        free(root);
    }
    
    // REGULAR
    else if(root->node_type == REGULAR){
        free_tree(root->children[0].qchild);
        free_tree(root->children[1].qchild);
        free(root->question);
        free(root);
    }
    
}







