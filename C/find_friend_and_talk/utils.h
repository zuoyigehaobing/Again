#ifndef _UTILS_H
#define _UTILS_H

#include <netinet/in.h>
#include "qtree.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

#define MAX_NAME 128
#define INPUT_ARG_MAX_NUM 3
#define DELIM " \r\n"

/*
 * Τhe definition of the following structure is copied directly from the
 * muffinman server (http://www.cdf.toronto.edu/~ajr/209/a4/muffinman.c).
 * You might need to add more fields - such as a seoarate buffer for each client and
 * the current position in the buffer - as explained in A 4 handout.
 */
typedef struct client {
    int fd;
    int * answers;
    // 0->name   1->done_question -1->log_out
    int state;
    char buf[MAX_LINE];
    int inbuf;
    struct in_addr ipaddr;
    char username[MAX_NAME];
    struct client *next;
} client;

/*
 * Print a formatted error message to stderr.
 */
void error(char *);
int validate_answer(char *answer,int clientfd);
int find_network_newline(char *buf,int inbuf);
int question_num(Node * head);
int * get_answer(char *username, QNode * root, int q_num);
int do_test(QNode **root, Node *interests,
            struct client *current_client, struct client *head);
void send_text(int clientfd,char* msg);

int disconnect_client(char*username,client**addr_online_clients);
void free_one_client(char * username,client * connected);
client * find_client(char*username,client*online_clients);
Node * get_opposite(char *username,QNode * root,int clientfd,int q_num);
/*
 * Read and process commands
 */
int process_args(int, char **, QNode **, Node *, struct client *, struct client *);


/*
 * Tokenize the string stored in cmd.
 * Return the number of tokens, and store the tokens in cmd_argv.
 */
int tokenize(char *, char **);

#endif /* _UTILS_H */
