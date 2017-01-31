#include "qtree.h"

void print_greetings();
int enter_username(char * username,QNode * root);
int validate_username(char * username,QNode * root);
int answer_question(char * answer,QNode * root);
int check_and_collect_answer(char * this_answer,char*total_answer,QNode * root,int level);



int main (int argc, char **argv) {
	
	QNode *root = NULL;	
	char answer[MAX_LINE];	
	char name[MAX_LINE];
	

	Node * interests = NULL;

	if (argc < 2) {
        printf ("To run the program ./categorizer <name of input file>\n");
        return 1;
    }
	
    interests = get_list_from_file (argv[1]);
	
    if (interests == NULL)
        return 1;

	root = add_next_level (root,  interests);
	free_list (interests);

	//main application loop
    int terminate = 0;
    
    while (1) {
        print_greetings();
        
        // STEP 1 : ENTER THE USERNAME AND CHECK IF IT'S VALID
        terminate = enter_username(name,root);
        if(terminate == 1)
            break;
        Node * head = search(name,root);
        
        // STEP 2 : ANSWER THE QUESTION AND ADD USER NAME
        if(head == NULL){
            terminate = answer_question(answer,root);
            if(terminate == 1)
                break;
            add_user_to_leaves(name,answer,root);
            head = search(name,root)->next;
        }
        
        
        // STEP 3 : go to STEP 4
        
        
        // STEP 4 : LIST YOUR FRIEND
        
        if(head == NULL){
            printf("Sorry, no user with similar interests joined yet\n");
        }
        if(head != NULL){
            printf("Friends recommendations for user : %s\n",name);
            print_list(head);
        }
        printf("\n");
    }
	
    //end of main loop - the user tyoped "q"
    print_qtree (root, 0);
    
    //free the tree
    free_tree(root);
    
	return 0;
}

void print_greetings () {
	printf ("----------------------------------------------\n");
	printf ("Friend recommender system. Find people who are just like you!\n");
	printf ("CSC209 fall 2016 team. All rights reserved\n");
	printf ("----------------------------------------------\n");
}



// ====================  ENTER USER NAME ============================

int enter_username(char * username,QNode * root){
    
    // enter the username
    printf("Please enter your name. Type 'q' to quit\n");
    fgets(username,MAX_LINE,stdin);
    username[strcspn(username,"\r\n")] = '\0';
    int check_username = validate_username(username,root);
    // if the user is invalid , go into the loop
    while(check_username != 0){
        
        if(check_username == 999){
            return 1;
        }
        fgets(username,MAX_LINE,stdin);
        username[strcspn(username,"\r\n")] = '\0';
        check_username = validate_username(username,root);
    }
    return 0;
    
}


int validate_username(char * username,QNode * root){
    int size = strlen(username);
    
    // if the user typed q here , quit
    if(strcmp(username,"q") == 0){
        printf("Quiting ...\n");
        return 999;
    }
    //check the length of the username
    if(size > 128 || size < 8){
        printf("Invalid length, please enter another one: \n");
        return -1;
    }
    //check each char in the name is aplhanumeric
    for(int i=0;i<size;i++){
        if(username[i] >= 'A' && username[i] <= 'Z'){
            continue;
        }
        else if(username[i] >= 'a' && username[i] <= 'z'){
            continue;
        }
        else if(username[i] >= '0' && username[i] <= '9'){
            continue;
        }
        else{
            printf("Invalid user name, please enter another one: \n");
            return -1;
        }
    }
    
    return 0;
    
    
}


// ====================== ANSWER AND COLLECT THE QUESTION ===============
int answer_question(char *answer,QNode *root){
    QNode * curr = root;
    int level = 0;
    char my_answer[MAX_LINE];
    int valid_answer = -1;
    
    
    while (curr != NULL) {
        printf("Do you like %s? (y/n)\n",curr->question);
        scanf("%s",my_answer);
        my_answer[strcspn(my_answer,"\r\n")] = '\0';
        
        valid_answer = check_and_collect_answer(my_answer,answer,curr,level);
        
        if(valid_answer == 999){
            printf("Quiting ...\n");
            return 1;
        }
        
        else if(valid_answer == 0){
            if(curr->node_type == LEAF){
                break;
            }
            curr = curr->children[0].qchild;
            level++;
        }
        }
    
    return 0;
}



int check_and_collect_answer(char * this_answer,char*total_answer,QNode * root,int level){
    
    // if the user types q or qx or qxx, quit
    
    // check the length of the answer:
    if(strlen(this_answer) > 3 || strlen(this_answer) <= 0){
        printf("Invalid answer length, please enter again\n");
        return -1;
    }
    
    // check the first character
    char check = this_answer[0];
    if(check == 'y'){
        total_answer[level] = '1';
    }
    
    else if(check == 'n'){
        total_answer[level] = '0';
    }
    
    else if(check == 'q'){
        return 999;
    }
    
    else{
        printf("Invalid answer , please try anagin \n");
        return -1;
    }
    
    return 0;
}











