#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/*
 * Print a formatted error message to stderr.
 */
void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

int validate_answer(char *answer,int clientfd){
    char *invalid_message = "ERROR: Answer must be one of 'y', 'n', 'quit'.\n";

    if (strlen(answer) > 10){
        printf("%s", invalid_message);
        return 2;
    }

    if (answer[0] == 'q' || answer[0] == 'Q')
        return -1;

    if (answer[0] == 'n' || answer[0] == 'N')
        return 0;

    if (answer[0] == 'y' || answer[0] == 'Y')
        return 1;
    char buf[MAX_LINE];
    sprintf(buf,"%sanswer : %s\n", invalid_message,answer);
    send_text(clientfd,buf);
    return 2;
}

/*
 * Read and process commands
 */
int process_args(int cmd_argc, char **cmd_argv, QNode **root, Node *interests,
		 struct client *current_client, struct client *head) {
	QNode *qtree = *root;

	if (cmd_argc <= 0) {
		return 0;

	} else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
		/* Return an appropriate value to denote that the specified
		 * user is now need to be disconnected. */
		return -1;

	} else if (strcmp(cmd_argv[0], "do_test") == 0 && cmd_argc == 1) {
		/* The specified user is ready to start answering questions. You
		 * need to make sure that the user answers each question only
		 * once.
		 */
         int check = do_test(root, interests,current_client,head);
         current_client -> state = check;
         return check;

	} else if (strcmp(cmd_argv[0], "get_all") == 0 && cmd_argc == 1) {
		/* Send the list of best mismatches related to the specified
		 * user. If the user has not taked the test yet, return the
		 * corresponding error value (different than 0 and -1).
		 */
         get_opposite(current_client->username,
                qtree,current_client->fd,question_num(interests));


	} else if (strcmp(cmd_argv[0], "post") == 0 && cmd_argc == 3) {
		/* Send the specified message stored in cmd_argv[2] to the user
		 * stored in cmd_argv[1].
		 */
	}
	else {
		/* The input message is not properly formatted. */
		send_text(current_client->fd,"Invalid command\n");
	}
	return 0;
}


// -1 for quit,  1 for done
int do_test(QNode **root, Node *interests,
            struct client *current_client, struct client *head){

    QNode * qtree = *root;
    Node * user_list = find_user(qtree,current_client->username);

    char buf[MAX_LINE];
    char buf2[MAX_LINE];
    char answer[MAX_LINE];
    int inbuf = 0;
    int clientfd = current_client->fd;
    //printf("Your user name : %s\n",current_client->username);
    if(user_list != NULL){
        //printf("Client fd :%d\n",clientfd);
        send_text(current_client->fd,"You already done the test\n");
        return 1;
    }
    if(user_list == NULL){
        QNode * prev,*curr;
        prev = curr = qtree;
        // collecting your interests :
        send_text(clientfd,"collecting your interests\n");


        // set up reading structure
        char * after = buf + inbuf;
        int room = sizeof(buf) - inbuf;
        int nbytes,where;
        // iterate over list of interests
        Node *i = interests;
        int ans;
        while(i != NULL){
            // should send the question to the client

            bzero(buf2,MAX_LINE);
            sprintf(buf2,"do you like %s?\n",i->str);
            send_text(clientfd,buf2);

            // should read the answer from the client
            while(1){
                if((nbytes = read(clientfd,after,room)) > 0){
                    inbuf += nbytes;
                    where = find_network_newline(buf,inbuf);
                    if(where >= 0){
                        buf[where] = '\0';
                        buf[where + 1] = '\0';
                        bzero(answer,MAX_NAME);
                        strncpy(answer,buf,where);
                        where += 2;
                        inbuf -= where;
                        memmove(buf,buf+where,inbuf);
                        // validate the username
                        if(strlen(answer) == 0){
                            room = sizeof(buf) - inbuf;
                            after = buf + inbuf;
                            continue;
                        }

                        ans = validate_answer(answer,clientfd);
                        //printf("I get your answer : %d\n",ans);
                        // if 1: find branch
                        // if 0: find branch
                        // if 2: print invalid_message
                        // if -1: ready to quit
                        if(ans == -1){
                            //printf("quiting\n");
                            // should remove the client from the list outside
                            return -1;
                        }
                        if(ans == 2){
                            room = sizeof(buf) - inbuf;
                            after = buf + inbuf;
                            break;
                        }

                        prev = curr;
                        curr = find_branch(curr,ans);
                        i = i->next;
                        room = sizeof(buf) - inbuf;
                        after = buf + inbuf;
                        break;

                    }
                    room = sizeof(buf) - inbuf;
                    after = buf + inbuf;
                }
                else if(nbytes == 0){
                    //printf("client Disconnect\n");
                    return -1;
                    break;
                }
            }

        }
        prev->children[ans].fchild =
            add_user(prev->children[ans].fchild,current_client->username);
        // should use find answer and update the answer of the client
        current_client->answers = get_answer(current_client->username,qtree,question_num(interests));
        send_text(clientfd,"Test Complete\n");
    }

    return 1;


}

/*
 * Tokenize the command stored in cmd.
 * Return the number of tokens, and store the tokens in cmd_argv.
 */
int tokenize(char *cmd, char **cmd_argv) {
    int cmd_argc = 0;
    char *next_token = strtok(cmd, DELIM);

    while (next_token != NULL) {
        cmd_argv[cmd_argc] = next_token;
        ++cmd_argc;

	if(cmd_argc < (INPUT_ARG_MAX_NUM - 1))
	    next_token = strtok(NULL, DELIM);
	else
	    break;
    }

    if (cmd_argc == (INPUT_ARG_MAX_NUM - 1)) {
	cmd_argv[cmd_argc] = strtok(NULL, "");
	if(cmd_argv[cmd_argc] != NULL)
	    ++cmd_argc;
    }

    return cmd_argc;
}


int find_network_newline(char *buf,int inbuf){
    int i;
    for (i = 0;i<inbuf -1;i++){
        if ((buf[i] == '\r') && (buf[i+1] == '\n')){
            return i;
        }
    }
    return -1;
}

int question_num(Node * head){
    Node * cur = head;
    int result = 0;
    while(cur != NULL){
        result ++;
        cur = cur->next;
    }
    return result;
}

int * get_answer(char * username,QNode * root,int q_num){
    QNode * cur;
    Node * check;
    cur = root;
    check = find_user(cur,username);

    if(!check){
        return NULL;
    }

    int * result = (int*)calloc(q_num,sizeof(int));
    int index = 0;

    while(cur->node_type != LEAF){
        check = find_user(cur->children[0].qchild,username);
        if (check == NULL){
            result[index] = 1;
            cur = cur->children[1].qchild;
            index ++;
        }
        else{
            result[index] = 0;
            cur = cur->children[0].qchild;
            index ++;
        }
    }
    if(find_user(cur,username) == cur->children[0].fchild){
        result[index] = 0;
    }
    else{
        result[index] = 1;
    }
    return result;
}

void send_text(int clientfd,char* msg){
    char buf[MAX_LINE];
    sprintf(buf,"%s",msg);
    int len;
    if((len = write(clientfd,buf,strlen(buf))) !=strlen(buf)){
        perror("writing");
        exit(1);
    }
}

int disconnect_client(char*username,client**addr_online_clients){
    client * head = *addr_online_clients;
    client * previous = NULL;
    while(head != NULL){
        if(strcmp(username,head->username) == 0){
            if(previous == NULL){
                free_one_client(head->username,*addr_online_clients);
                *addr_online_clients = NULL;
                return 0;
            }
            else{
                previous->next = head->next;
                free_one_client(head->username,*addr_online_clients);
                return 0;
            }
        }
        previous = head;
        head = head->next;

    }
    return 1;
}

void free_one_client(char * username,client * connected){
    client * this_client = find_client(username,connected);
    if(this_client != NULL){
        close(this_client->fd);
        free(this_client->answers);
        free(this_client);
    }
}

client * find_client(char*username,client*online_clients){
    client* head = online_clients;
    client * target = NULL;
    while(head != NULL){
        if(strcmp(head->username,username) == 0){
            target = head;
            break;
        }
        head = head->next;
    }
    return target;
}

// new part
Node * get_opposite(char *username,QNode * root,int clientfd,int q_num){
    QNode * cur;
    Node * check;
    cur = root;
    check = find_user(cur,username);
    Node * result = NULL;
    if(!check){
        send_text(clientfd,"You have to do the test first\n");
        return result;
    }

    int * answer = get_answer(username,root,q_num);
    int index = 0;
    while(cur->node_type != LEAF){
        cur = find_branch(cur,1-answer[index]);
        index ++;
    }

    result = cur->children[1-answer[index]].fchild;
    char*no = "No completing personality found please try again later\n";
    if(result == NULL){
        send_text(clientfd,no);
        return result;
    }

    char*yes = "Here are your best mismatches:\n";
    send_text(clientfd,yes);
    check = result;
    char namebuf[MAX_NAME];
    while(check != NULL){
        sprintf(namebuf,"%s\n",check->str);
        send_text(clientfd,namebuf);
        check = check->next;
    }
    send_text(clientfd,"\n");
    return result;
}
