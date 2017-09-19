#include "ext2.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>



#define LineMax 256

typedef struct path{
    char * name;
    char type;
    struct path * next;
} path;



path* pathNode(char * name, char type);
path * pathLink(char * disk_path);
char * filenameObtain(char * native_path);
void freepath(path * head);
void set_inode_bitmap(int offset,int val);
void set_block_bitmap(int offset,int val);
int path_walk(path * mypath);
int check_existence(char * native_file_name,int parent_inodedx);
int alignname(char * name);
int alignint(int namelen);
void initialize(int diskfd);
void freeAll();



//
void get_data_block(int *container,int block_count,int parent_inodedx);
int remove_from_directory(int parent_inodedx, char * native_file_name);
//

//===================== function for remove bonus
int remove_dir(int index);
void unset_inode(int index);
int is_filename(struct ext2_dir_entry * dir);

// ==================== function for restore
int restore_to_directory(int parent_inodedx, char * native_file_name,int restoreDir);
void increaseLink(int index);
int check_restoreable(struct ext2_dir_entry * dir);
void reset_inode(int index);
void get_dir_name(struct ext2_dir_entry * dir);

// =================== function for restore bonus
int restore_dir(int index);


// the helper function for cp
int check_is_dir(char * path);


// the helper function for mkdir
void initialize_new_dir(int block_idx, int index,int pindex);

// ln
int isDir(int inodeindex);
