#include "utils.h"




int validate_user(char *name);
// shan
int get_username(char *name, int clientfd);
int add_client(char*username,client* online_clients,int c_fd,struct in_addr c_addr,int q_num);
client * a_new_client(char*username,int c_fd,struct in_addr c_addr,int q_num);
int online_num(client*online_clients);
char ** cmd_initializer(int len);



QNode *root = NULL;
Node *interests = NULL;
client * connected = NULL;

char *user_prompt = "What is your name?\n";
char *welcome = "Welcome.\n";
char *what_to_do = "Go ahead and enter user commands>\n";
char *question_prompt = "Do you like %s? (y/n)\n";
char *neg_result = "Sorry, no users with similar interests joined yet\n\n";
char *pos_result1 = "friend recommendation for user %s:\n";
char *pos_result2 = "You have total %d potential friend(s)!!!\n\n";

int main (int argc, char **argv) {

    // ============== initialize all the overall variables
    char name[MAX_NAME];
    int q_num;


    // ============== check the number of command line arguments
    if (argc < 2) {
        printf ("To run the program ./categorizer <name of input file>\n");
        return 1;
    }

    // ============== read interests from file and built question tree
    interests = get_list_from_file (argv[1]);
    q_num = question_num(interests);
    if (interests == NULL)
        return 1;
    root = add_next_level (root,  interests);

    // ============== initialize the server and client socket
    int parentfd,clientfd;
    socklen_t clientlen;
    struct sockaddr_in serveraddr,clientaddr;
    char buf[MAX_LINE];
    int inbuf = 0;
    short port = (short) atoi(argv[2]);
    struct hostent *hostp;
    char*hostaddrp;
    int on = 1,status;

    // socket and status
    if((parentfd = socket(AF_INET,SOCK_STREAM,0))< 0){
        perror("socket");
        exit(1);
    }

    status = setsockopt(parentfd,SOL_SOCKET,SO_REUSEADDR,(void*)&on,sizeof(on) );
    if(status == -1){
        perror("socketopt -- REUSEADDR");
        exit(1);
    }

    bzero((char*)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // port from command line so that easy to debug,. need to clean after use
    serveraddr.sin_port = htons(port);

    // bind and listen
    if (bind(parentfd,(struct sockaddr *)&serveraddr,sizeof serveraddr) < 0){
        perror("bind");
        exit(1);
    }
    if (listen(parentfd,5)){
        perror("listen");
        exit(1);
    }

    printf("Listening on %d\n",port);



    // ============================== main application loop
    clientlen = sizeof clientaddr;
    client * current_client = NULL;
    char ** cmd;
    int clean = 0;

    for (; ;){
        // accepting a client
        if((clientfd = accept(parentfd,(struct sockaddr *)&clientaddr,&clientlen)) < 0){
            perror("accept");
            exit(1);
        }

        // print the client which server established connetction with
        hostp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr,
                    sizeof(clientaddr.sin_addr.s_addr),AF_INET);
        if(hostp == NULL){
            perror("gethostbyaddr");
            exit(1);
        }
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if(hostaddrp == NULL){
            perror("inet_ntoa");
            exit(1);
        }
        printf("Connection from %s %s\n",
            hostp->h_name,hostaddrp);


        // get a valid username , check whether it's quit
        clean = get_username(name,clientfd);
        if (clean == -1){
            close(clientfd);
            printf("Removeing connection from %s\n",inet_ntoa(clientaddr.sin_addr));
            continue;
        }
        printf("Adding client %s\n",inet_ntoa(clientaddr.sin_addr));
        if(connected == NULL){
            connected = a_new_client(name,clientfd,clientaddr.sin_addr,q_num);
            //printf("online clients : %d\n",online_num(connected));
        }
        else if(connected != NULL && find_client(name,connected) == NULL){
            add_client(name,connected,clientfd,clientaddr.sin_addr,q_num);
            //client * c = find_client("bingzhao" ,connected);
            //printf("I've found : %s\n",c->username);
            //printf("online clients : %d\n",online_num(connected));
        }
        //add_client(name,connected,clientfd,inet_ntoa(q.sin_addr));
        current_client = find_client(name,connected);
        // update fd
        current_client->fd = clientfd;


        // for test : passed
        //printf("Received username : %s\n", current_client->username);


        // asking what to do
        send_text(clientfd,welcome);
        // can do multiple commands
        while(1){
            send_text(clientfd,what_to_do);
            cmd = cmd_initializer(MAX_LINE);
            bzero(buf,MAX_LINE);
            inbuf = 0;
            char * after = buf + inbuf;
            int room = sizeof(buf) - inbuf;
            int nbytes,where;
            char container[MAX_LINE];
            // go out until get a complete command line
            int terminate_cmd = 0;
            while(1){
                // receive user commands
                if((nbytes = read(clientfd,after,room)) > 0){
                    inbuf += nbytes;
                    where = find_network_newline(buf,inbuf);
                    if(where >= 0){
                        buf[where] = '\0';
                        buf[where + 1] = '\0';
                        bzero(container,MAX_NAME);
                        strncpy(container,buf,where);
                        where += 2;
                        inbuf -= where;
                        memmove(buf,buf+where,inbuf);
                        if(strlen(container) != 0){
                            //printf("My container : %s\n",container);
                            break;
                        }
                    }
                    room = sizeof(buf) - inbuf;
                    after = buf + inbuf;
                }
                else if(nbytes == 0){
                    //printf("client Disconnect from exit 1\n");
                    terminate_cmd = -1;
                    break;
                }
            }
            if(terminate_cmd == -1){
                printf("Removeing connection from %s\n",inet_ntoa(clientaddr.sin_addr));
                disconnect_client(current_client->username,&connected);
                break;
            }
            // hanged process user commands
            int cmd_num = tokenize(container,cmd);
            terminate_cmd = process_args(cmd_num, cmd, &root, interests,current_client, connected);
            printf("recorded activity for %s\n",current_client->username);
            if(terminate_cmd == -1){
                printf("Removeing connection from %s\n",inet_ntoa(clientaddr.sin_addr));
                disconnect_client(current_client->username,&connected);
                break;
            }
        }


        bzero(buf,MAX_LINE);






    }

    return 0;
}










// =============================


// 0 for valid, -1 for quit
int get_username(char *name, int clientfd){
    char buf[MAX_LINE];
    int inbuf = 0;
    // write user_prompt
    send_text(clientfd,user_prompt);

    bzero(name,MAX_NAME);

    //
    char *after = buf + inbuf;
    int room = sizeof(buf) - inbuf;
    int nbytes,where;

    while(1){
        if((nbytes = read(clientfd,after,room)) > 0){
            inbuf += nbytes;
            where = find_network_newline(buf,inbuf);
            if(where >= 0){
                buf[where] = '\0';
                buf[where + 1] = '\0';
                bzero(name,MAX_NAME);
                strncpy(name,buf,where);
                where += 2;
                inbuf -= where;
                memmove(buf,buf+where,inbuf);
                // validate the username
                int check = validate_user(name);
                if(check == 1){
                    break;
                }
                if(check == -1){
                    printf("client Disconnection 1\n");
                    return -1;
                    // should remove the client from the linked list
                }
                send_text(clientfd,"Invalid username, please try  another: \n");
            }
            room = sizeof(buf) - inbuf;
            after = buf + inbuf;
        }
        else if(nbytes == 0){
            //printf("client Disconnection 2\n");
            return -1;
            break;
        }

        // clean buf and  read user name, and add the user

    }
    return 0;
}


// =============================







































int validate_user(char *name){
    int valid = 1;

    int len = strlen(name);
    if (len < 8 || len > 128){
        valid = 0;
    }

    for (int i = 0; i < len; i++){
        if (!isalnum(name[i])){
            valid = 0;
            break;
        }
    }

    if(strcmp(name,"quit") == 0){
        valid = -1;
    }
    return valid;
}





// ============================= shan




client * a_new_client(char*username,int c_fd,struct in_addr c_addr,int q_num){
    client * new_client = (client*)calloc(1,sizeof(client));
    new_client->next = NULL;
    new_client->fd = c_fd;
    new_client->ipaddr = c_addr;
    new_client->state = 0;
    new_client->inbuf = 0;
    new_client->answers = (int*)calloc(q_num,sizeof(int));
    strncpy(new_client->username,username,strlen(username));
    return new_client;
}

int add_client(char*username,client* online_clients,int c_fd,struct in_addr c_addr,int q_num){
    client * head = online_clients;

    while(head->next != NULL){
        if(strcmp(head->username,username) == 0){
            return 0;
        }
        head = head->next;
    }
    client * new_client = (client*)calloc(1,sizeof(client));
    new_client->next = NULL;
    new_client->fd = c_fd;
    new_client->ipaddr = c_addr;
    new_client->state = 0;
    new_client->inbuf = 0;
    new_client->answers = (int*)calloc(q_num,sizeof(int));
    strncpy(new_client->username,username,strlen(username));
    head->next = new_client;
    return 1;
}



int online_num(client*online_clients){
    client * head = online_clients;
    int result = 0;
    while(head != NULL){
        result ++;
        head = head->next;
    }
    return result;
}

char ** cmd_initializer(int len){
    char ** cmd = (char **) calloc(len,sizeof(char*));
    for(int i=0;i<len;i++){
        cmd[i] = (char *)calloc(len,sizeof(char));
    }
    return cmd;
}



// ================= new part
