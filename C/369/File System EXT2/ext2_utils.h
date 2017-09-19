#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ext2.h"

#ifndef EXT2_UTILS_H
#define EXT2_UTILS_H

typedef struct node{
    char name[EXT2_NAME_LEN + 1];
    struct node * next;
} Node;

typedef struct {
	Node * head;
	Node * tail;
	Node * cur;
	int size;
} Linkedlist;

unsigned char *disk;
struct ext2_super_block *sb;
struct ext2_group_desc *gd;
unsigned char *block_bm,  *inode_bm;
struct ext2_inode * inode_tbl;

//===============================disk===============================
int init_disk_ptrs(char * disk_file);
unsigned char * get_block_ptr(int block_num);
struct ext2_inode * get_inode_ptr(int inode_num);

//===============================bitmap===============================
void set_inode_bitmap(int inode_num, int val);
void set_block_bitmap(int block_num,int val);
int get_free_inode_num();
int get_free_block_num();
int get_inode_bm_bit(int inode_num);
int get_block_bm_bit(int block_num);
int get_free_blocks(int * container,int block_count);

//=================================Linkedlist=================================
//----------------------Linkedlist Creation----------------------
Linkedlist * path_link(char * disk_path);
void init_linked_list(Linkedlist * llist);
void add_new_node(Linkedlist * llist, char * name);

//----------------------Linkedlist Traversal----------------------
void init_llist_traverse(Linkedlist * llist);
Node * get_next_node(Linkedlist * llist);

//----------------------Linkedlist Destroying----------------------
void free_llist(Linkedlist * llist);

//========================Search a directory========================
int search_dir(int inode_num, char *name);
int search_dir_block(int block_num, char *name);

//==============================Add dir entry=====================================
int add_dir_entry_block(int block_num, int entry_inode_num, char type, 
		char *name, int file_blocks);
void add_dir_entry(int dir_inode_num, int entry_inode_num, char type, 
		char *name, int file_blocks);

//=======================File size related functions=======================
int get_src_file_size(char * filename);
int blocks_needed(int filesize);

//===========================inode==================================
void init_inode(int inode_num,unsigned short type, int total_size, 
	int* target_blocks);
void get_data_block(int *container,int block_count,int inode_num);

//===========================dir entry===========================
void init_dir_entry(struct ext2_dir_entry * dir_entry, 
		int inode_num, int rec_len, char *file_name, char file_type);

//===============================path walks===============================
int find_path_inode(Linkedlist * llist, char * src_path, int end_slash, 
	char * target_name);
int find_dir_inode(Linkedlist * llist, char * target_name);
int find_file_inode(Linkedlist * llist, int is_sym_link, int end_slash);
int find_rm_inode(Linkedlist * llist, int end_slash, int *target_inode, 
	char *target);
void traverse_path(Linkedlist * llist, int * second_last_inode, 
	int * last_inode);

//==============================types conversions==============================
char get_inode_type(int inode_num);
char get_dir_entry_type(unsigned char file_type);
unsigned char convert_file_type(char type);

//=============================Small tools=============================
//---------------------traversing---------------------
void get_entry_name(char * dir_entry_name, struct ext2_dir_entry * dir_entry);
struct ext2_dir_entry * get_next_entry(struct ext2_dir_entry *dir_entry);
int is_same_block(struct ext2_dir_entry * cur_entry, int block_num);
int get_rec_len(int len);

//----------------------file name/type--------------------------
int end_with_slash(char *path);
void get_basename(char *src_path, char *file_name);

//=========================my system calls=========================
void my_close(int fd);
void * my_calloc(size_t num, size_t size);

//=======================Helper for cp=======================
int src_not_dir(char * path);
int src_file_size(char * filename);
int find_path_inode(Linkedlist * llist, char * src_path, int end_slash, 
	char * target_name);
void get_basename(char *src_path, char *file_name);
void copy_each_block(int native_file_size,int native_fd, 
	int block_count, int * target_blocks);

//=======================Helper for mkdir=======================
void init_dir(int block_num, int inode_num, int parent_inode_num);

//=======================Helper for rm=======================
int del_file(int inode_num, char *file_name);
int del_file_block(int block_num, char *file_name);
void remove_file(int inode_num);

//========================Helper for restore========================
int search_block_restore(int inode_num, char *file_name);
struct ext2_dir_entry * find_prev_entry(int block_num, struct ext2_dir_entry ** prev_entry, 
	struct ext2_dir_entry * cur_entry);
int search_dir_restore(int inode_num, char *file_name);
int try_restore_inode(int inode_num);

#endif
