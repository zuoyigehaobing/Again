#include <string.h>
#include <stdio.h>
#include "ext2_utils.h"

//=============================disk=============================
/* Initialize global variables for disk,
 * Includes return value checking when opening img file.
 */
int init_disk_ptrs(char * disk_file) {
	int fd = open(disk_file, O_RDWR);

	if(fd == -1){
		perror("error opening file");
		exit(ENOENT);
	}

	disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	sb = (struct ext2_super_block *)get_block_ptr(1);
	gd = (struct ext2_group_desc *)get_block_ptr(2);
	block_bm = get_block_ptr(gd->bg_block_bitmap);
	inode_bm = get_block_ptr(gd->bg_inode_bitmap);
	inode_tbl = (struct ext2_inode *)get_block_ptr(gd->bg_inode_table);

	return fd;
}

/* Return a pointer to the block given by its block number */
unsigned char * get_block_ptr(int block_num) {
	return disk + ((block_num) * EXT2_BLOCK_SIZE);
}

/* Return a pointer to the inode given by its inode number */
struct ext2_inode * get_inode_ptr(int inode_num) {
	return inode_tbl + (inode_num - 1);
}


//=============================bitmap================================
/* If the corresponding bitmap bit of the inode is 1, this function returns
 * a non-zero value; otherwise, returns 0.
 */
int get_inode_bm_bit(int inode_num) {
	int idx = inode_num - 1;
	int byte_num = idx / 8;
	int byte_offset = idx % 8;
	unsigned char *byte = inode_bm + byte_num;
	return *byte & 1 << byte_offset;
}

/* If the corresponding bitmap bit of the block is 1, this function returns
 * a non-zero value; otherwise, returns 0.
 */
int get_block_bm_bit(int block_num) {
	int idx = block_num - 1;
	int byte_num = idx / 8;
	int byte_offset = idx % 8;
	unsigned char *byte = block_bm + byte_num;
	return *byte & 1 << byte_offset;

}

/* Set the bitmap bit of the given inode to val,
 * and update superblock's & group desc's free inodes count.
 */
void set_inode_bitmap(int inode_num, int val){
	if (!!get_inode_bm_bit(inode_num) == val)
		return;

	int offset = inode_num - 1;
    int byte = offset / 8;
    int bit = offset % 8;
    unsigned char * target = inode_bm + byte;
    *target ^= (-val ^ *target) & (1 << bit);
    gd->bg_free_inodes_count += 1 - 2*val;
    sb->s_free_inodes_count += 1-2*val;
}

/* Set the bitmap bit of the given block to val,
 * and update superblock's & group desc's free blocks count.
 */
void set_block_bitmap(int block_num, int val){
	if (!!get_block_bm_bit(block_num) == val)
		return;

	int offset = block_num - 1;
    int byte = offset / 8;
    int bit = offset % 8;
    unsigned char * target = block_bm + byte;
    *target ^= (-val ^ *target) & (1 << bit);
    gd->bg_free_blocks_count += 1 - 2*val;
    sb->s_free_blocks_count += 1 - 2*val;

}

/* Return the first free inode number */
int get_free_inode_num() {
	int i;
	for (i = 0; i < sb->s_inodes_count; i++)
		if (!get_inode_bm_bit(i + 1))
			return i + 1;
	return -1;
}

/* Return the first free block number */
int get_free_block_num() {
	int i;
	for (i = 0; i < sb->s_blocks_count; i++)
		if (!get_block_bm_bit(i + 1))
			return i + 1;
	return -1;
}

/* Get block_count free blocks and put them into the coninter,
 * return 0 if successful
 */
int get_free_blocks(int * container,int block_count){
	int i, cur_idx = 0;
	for(i = 0; i < sb->s_blocks_count; i++) {
		if(get_block_bm_bit(i + 1) == 0){
			container[cur_idx] = i;
			cur_idx++;
			set_block_bitmap(i + 1, 1);
			if(cur_idx == block_count)
				break;
		}
	}

	if(cur_idx != block_count)
		return -1;

	return 0;
}


//========================linked list operations============================

//----------------------Linkedlist Creation----------------------
/* Create a Linklist of the path, where each Node is a file name */
Linkedlist * path_link(char * disk_path){
	char cur_name[EXT2_NAME_LEN + 1];
	int i, path_len = strlen(disk_path);
	Linkedlist * llist = (Linkedlist *)my_calloc(1, sizeof(Linkedlist));

	memset(cur_name, '\0', EXT2_NAME_LEN + 1);
	for(i = 1; i < path_len; i++){
		if (disk_path[i] == '/') {
			add_new_node(llist, cur_name);
			memset(cur_name, '\0', EXT2_NAME_LEN + 1);
			continue;
		} else
			strncat(cur_name, &disk_path[i], sizeof(char));

		if (i == path_len - 1) {
			add_new_node(llist, cur_name);
		}
	}

	return llist;
}

/* Initialize a new Linkedlist */
void init_linked_list(Linkedlist * llist) {
	llist->head = NULL;
	llist->tail = NULL;
	llist->cur = NULL;
	llist->size = 0;
}

/* Create a new node and add it to llist */
void add_new_node(Linkedlist * llist, char * name){
	Node * cur = (Node*)my_calloc(1, sizeof(Node));
	strncpy(cur->name, name, EXT2_NAME_LEN);
	cur->name[EXT2_NAME_LEN] = '\0';

	if (llist->size == 0)
	{
		llist->head = cur;
		llist->tail = cur;
	} else {
		llist->tail->next = cur;
		llist->tail = cur;
	}
	llist->size++;
}

//----------------------Linkedlist Traversal----------------------
/* Initialize the cur pointer, i.e., make it point to the head */
void init_llist_traverse(Linkedlist * llist) {
	llist->cur = llist->head;
}

/* Move the cur pointer to the next Node */
Node * get_next_node(Linkedlist * llist) {
	llist->cur = llist->cur->next;
	return llist->cur;
}

//----------------------Linkedlist Destroying----------------------
/* Free the Linkedlist */
void free_llist(Linkedlist * llist){
    Node * cur = llist->head;
    Node * next;
    while(cur != NULL){
        next = cur->next;
        free(cur);
        cur = next;
    }
    free(llist);
}


//========================Search a directory========================
/* The following two functions are always used together,
 * to search a directory with inode number inode_num for the file
 * with a file name called name.
 */
int search_dir(int inode_num, char *name) {
	struct ext2_inode * inode = get_inode_ptr(inode_num);
	int result;
	int i;
	for (i = 0; i < 12; ++i)//i_blocks
		if (inode->i_block[i]) {
			result = search_dir_block(inode->i_block[i], name);
			if (result != -1)
				return result;
		}

	return -1;

}

int search_dir_block(int block_num, char *name) {
	unsigned char *dir_block = get_block_ptr(block_num);
	struct ext2_dir_entry * cur_entry = (struct ext2_dir_entry *) dir_block;

	char dir_entry_name[EXT2_NAME_LEN + 1];
	while (is_same_block(cur_entry, block_num))
	{
		get_entry_name(dir_entry_name, cur_entry);

		if (strcmp(dir_entry_name, name) == 0)
			return cur_entry->inode;

		cur_entry = get_next_entry(cur_entry);
	}
	return -1;
}


//==============================Add dir entry==============================
/* The following two functions are always used together,
 * to add a directory entry to a parent directory
 */
void add_dir_entry(int dir_inode_num, int entry_inode_num, char type, 
		char *name, int file_blocks) {
	struct ext2_inode * inode = get_inode_ptr(dir_inode_num);
	int num;
	int i;
	for (i = 0; i < 12; ++i)
		if (inode->i_block[i] != 0 && inode->i_block[i+1] == 0)
		{
			num = add_dir_entry_block(inode->i_block[i], entry_inode_num, type, 
					name, file_blocks);
			if (num != inode->i_block[i] && i < 12)
			{
				inode->i_block[i + 1] = num;
				inode->i_blocks += 2;
				inode->i_size += EXT2_BLOCK_SIZE;
				set_block_bitmap(num, 1);
			}
			break;
		}
}

int add_dir_entry_block(int block_num, int entry_inode_num, char type, 
		char *name, int file_blocks) {
	struct ext2_dir_entry * cur_entry;
	struct ext2_dir_entry * prev_entry;
	int new_rec_len;

	if (block_num) {
		cur_entry = (struct ext2_dir_entry *)get_block_ptr(block_num);
		prev_entry = (struct ext2_dir_entry *)get_block_ptr(block_num);

		cur_entry = get_next_entry(prev_entry);
		while ((unsigned char *)cur_entry < get_block_ptr(block_num) + EXT2_BLOCK_SIZE)
		{
			cur_entry = get_next_entry(cur_entry);
			prev_entry = get_next_entry(prev_entry);
		}

		if ((prev_entry->rec_len - get_rec_len(prev_entry->name_len)) 
			>= get_rec_len(strlen(name)))
		{
			cur_entry = (struct ext2_dir_entry *)
				((void *)prev_entry + get_rec_len(prev_entry->name_len));
			new_rec_len = prev_entry->rec_len - get_rec_len(prev_entry->name_len);
			init_dir_entry(cur_entry, entry_inode_num, new_rec_len, name, type);

			prev_entry->rec_len = get_rec_len(prev_entry->name_len);
			return block_num;
		} else {
			if ((sb->s_free_blocks_count - 1) < file_blocks)
			{
				perror("No enough blocks");
				exit(ENOMEM);
			}

			int new_block = get_free_block_num();

			cur_entry = (struct ext2_dir_entry *)get_block_ptr(new_block);
			init_dir_entry(cur_entry, entry_inode_num, EXT2_BLOCK_SIZE, name, type);
			return new_block;
		}
	}
	return 0;
}


//=======================File size related functions=======================
/* Calculate the file size */
int get_src_file_size(char * filename){
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

/* Return the number of blocks needed by this filesize */
int blocks_needed(int filesize){
	int rval;
	if(filesize % EXT2_BLOCK_SIZE == 0)
		rval = filesize / EXT2_BLOCK_SIZE;
	else
		rval = filesize / EXT2_BLOCK_SIZE + 1;
	if(rval > 12)
		rval++;
	return rval;
}


//==========================inode==============================
/* Initialize the inode attributes. If given allocated blocks, 
 * then assign each allocated block to the curnode 
 * and set the block count
 */
void init_inode(int inode_num, unsigned short type, int total_size, int *target_blocks){
	int block_count = blocks_needed(total_size);
	struct ext2_inode * curnode = get_inode_ptr(inode_num);
	int i;
	int * indirect;
	time_t current_time = time(NULL);
	
	curnode->i_uid = 0;
	curnode->i_gid = 0;
	curnode->osd1 = 0;
	curnode->i_file_acl = 0;
	curnode->i_dir_acl = 0;
	curnode->i_faddr = 0;
	curnode->i_mode = type;
	curnode->i_links_count = 1;
	curnode->i_blocks = 0;
	curnode->i_size = total_size;
	curnode->i_ctime = (unsigned int)current_time;
	curnode->i_dtime = 0;

	for(i = 0; i < 15; i++)
		curnode->i_block[i] = 0;

	for(i = 0; i < 3; i++)
		curnode->extra[i] = 0;

	if (target_blocks != NULL) {
		curnode->i_blocks = block_count * 2;
		for(i = 0; i < block_count; i++) {
			if (i <= 12) {
				curnode->i_block[i] = target_blocks[i] + 1;
			} else {
				indirect = (int *)get_block_ptr(target_blocks[12] + 1);
				indirect += i - 13;
				*indirect = target_blocks[i] + 1;
			}
		}
	}

}

/* Extract all block number used by the file to a container */
void get_data_block(int *container,int block_count,int inode_num){
	int *indirect_block = NULL; 
	int i;
	struct ext2_inode * inode = get_inode_ptr(inode_num);
	for(i = 0; i < block_count; i++){
		if (i < 12) {
			container[i] = inode->i_block[i];
		} else {
			if (indirect_block == NULL)
				indirect_block = (int *)get_block_ptr(inode->i_block[12]);

			container[i] = *indirect_block;
			indirect_block++;
		}
	}
}


//===========================dir entry===========================
/* Initialize a directory entry */
void init_dir_entry(struct ext2_dir_entry * dir_entry, 
		int inode_num, int rec_len, char *file_name, char file_type) {
	dir_entry->inode = inode_num;
	dir_entry->rec_len = rec_len;
	dir_entry->name_len = strlen(file_name);
	dir_entry->file_type = convert_file_type(file_type);
	strncpy(dir_entry->name, file_name, strlen(file_name));
}


//===============================path walks===============================
/* The following funcitons are used to do different path walks for 
 * different operations.
 */
int find_path_inode(Linkedlist * llist, char * src_path, int end_slash, 
		char * target_name) {
	char src_basename[EXT2_NAME_LEN + 1];
	get_basename(src_path, src_basename);

	int second_last_inode = 2;
	int last_inode;

	if (llist->size == 0)
	{
		strncpy(target_name, src_basename, EXT2_NAME_LEN);

		if (search_dir(second_last_inode, target_name) != -1)
		{
			perror("File already exists");
			exit(EEXIST);
		}

		target_name[strlen(src_basename)] = '\0';
		return 2;
	} else {
		traverse_path(llist, &second_last_inode, &last_inode);
		if (last_inode != -1)
		{
			if (get_inode_type(last_inode) == 'd')
			{
				strncpy(target_name, src_basename, EXT2_NAME_LEN);

				if (search_dir(last_inode, target_name) != -1)
				{
					perror("File already exists");
					exit(EEXIST);
				}

				target_name[EXT2_NAME_LEN] = '\0';
				return last_inode;
			} else {
				perror("File alreay exists");
				exit(EEXIST);
			}
		} else if (second_last_inode != -1) {
			if (end_slash)
			{
				perror("No such directory");
				exit(ENOENT);
			} else if (get_inode_type(second_last_inode) != 'd')  {
				perror("Not a directory");
				exit(ENOTDIR);
			} else {
				strncpy(target_name, llist->tail->name, EXT2_NAME_LEN);
				return second_last_inode;
			}
		}
	}
	return -1;
}

int find_dir_inode(Linkedlist * llist, char * target_name) {
	int second_last_inode;
	int last_inode;

	traverse_path(llist, &second_last_inode, &last_inode);

	if (last_inode != -1)
	{
		perror("File or directory already exists");
		exit(EEXIST);
	} else if (second_last_inode != -1) {
		if (get_inode_type(second_last_inode) != 'd')
		{
			perror("Not a directory");
			exit(ENOTDIR);
		} else {
			strncpy(target_name, llist->tail->name, EXT2_NAME_LEN);
			target_name[EXT2_NAME_LEN] = '\0';
			return second_last_inode;
		}
	}
	return -1;
}

int find_file_inode(Linkedlist * llist, int is_sym_link, int end_slash) {
	int second_last_inode;
	int last_inode;

	traverse_path(llist, &second_last_inode, &last_inode);


	if (last_inode == -1)
	{
		perror("No such file or directory");
		exit(ENOENT);
	} else {
		char type = get_inode_type(last_inode);
		if (type == 'f')
		{
			if (end_slash)
			{
				perror("Not a directory");
				exit(ENOTDIR);
			}
		} else {
			if (!is_sym_link)
			{
				perror("Not a regular file");
				exit(EISDIR);
			}
		}
		return last_inode;
	}
	return -1;
}

int find_rm_inode(Linkedlist * llist, int end_slash, int *target_inode, char *target) {
	int second_last_inode;
	int last_inode;
	*target_inode = -1;

	traverse_path(llist, &second_last_inode, &last_inode);

	if (last_inode == -1 || second_last_inode == -1)
	{
		perror("No such file or directory");
		exit(ENOENT);
	} else {
		char type = get_inode_type(last_inode);
		if (type == 'd')
		{
			perror("Is a directory");
			exit(EISDIR);
		} else {
			if (end_slash)
			{
				perror("Not a directory");
				exit(ENOTDIR);
			}
		}
		strncpy(target, llist->tail->name, EXT2_NAME_LEN);
		*target_inode = last_inode;
		return second_last_inode;
	}
	return -1;
}

/* Traverse the path in llist, return the inode numbers for the 
 * last file name and the second last file name if possible;
 * otherwise, return -1.
 */
void traverse_path(Linkedlist * llist, int * second_last_inode, int * last_inode) {
	init_llist_traverse(llist);
	*second_last_inode = 2;
	*last_inode = -1;
	while (llist->cur) {
		if (*second_last_inode != -1 && get_inode_type(*second_last_inode) == 'd') {
			if (llist->cur == llist->tail)
				*last_inode = search_dir(*second_last_inode, llist->cur->name);
			else
				*second_last_inode = search_dir(*second_last_inode, llist->cur->name);
		}

		get_next_node(llist);
	}
}


//==============================types conversions==============================
/* Return the type represented by a char for an inode */
char get_inode_type(int inode_num) {
	struct ext2_group_desc *gd = (struct ext2_group_desc *)get_block_ptr(2);
	struct ext2_inode *inode = (struct ext2_inode *)get_block_ptr(gd->bg_inode_table);
	inode += inode_num - 1;

	int mode = inode->i_mode & 0xF000;
	char type;
	if (mode == EXT2_S_IFDIR)
		type = 'd';
	else if (mode == EXT2_S_IFREG)
		type = 'f';
	else if (mode == EXT2_S_IFLNK)
		type = 's';
	return type;
}

/* Return the type represented by a char for a directory entry */
char get_dir_entry_type(unsigned char file_type) {
	char type;
	if (file_type == EXT2_FT_DIR)
		type = 'd';
	else if (file_type == EXT2_FT_REG_FILE)
		type = 'f';
	else if (file_type == EXT2_FT_SYMLINK)
		type = 's';
	return type;
}

/* Convert a char back to directory entry type */
unsigned char convert_file_type(char type) {
	unsigned char ext2_type;
	switch(type) {
		case 'd':
			ext2_type = EXT2_FT_DIR;
			break;
		case 'f':
			ext2_type = EXT2_FT_REG_FILE;
			break;
		case 's':
			ext2_type = EXT2_FT_SYMLINK;
			break;
		default:
			ext2_type = EXT2_FT_UNKNOWN;
	}
	return ext2_type;

}


//=============================Small tools=============================

//---------------------traversing---------------------
/* Get the rec_len given the name_len len for a directory entry */
int get_rec_len(int len) {
	int name_len = 0;
	while (name_len < len)
		name_len += 4;

	return sizeof(struct ext2_dir_entry) + name_len;
}

/* Check if the cur_entry pointer is still in the original block */
int is_same_block(struct ext2_dir_entry * cur_entry, int block_num) {
	unsigned char *block = get_block_ptr(block_num);
	return (unsigned char *)cur_entry < block + EXT2_BLOCK_SIZE;
}

/* Move dir_entry to the next entry */
struct ext2_dir_entry * get_next_entry(struct ext2_dir_entry *dir_entry) {
	return (struct ext2_dir_entry *)((void *)dir_entry + dir_entry->rec_len);
}

/* Copy the name from the directory entry to an array */
void get_entry_name(char * dir_entry_name, struct ext2_dir_entry * dir_entry) {
	int i;
	for (i = 0; i < dir_entry->name_len; i++)
		dir_entry_name[i] = dir_entry->name[i];
	dir_entry_name[dir_entry->name_len] = '\0';
}

//----------------------file name/type--------------------------
/* Extract the basename from a path src_path */
void get_basename(char *src_path, char *file_name) {
	int i;
	int count = 0;
	i = strlen(src_path) - 1;
	while (i >= 0) {
		if (src_path[i] == '/')
			break;
		i--;
		count++;
	}
	i++;
	int j;
	for (j = 0; j < count + 1; j++)
		file_name[j] = src_path[i + j];
}

/* Check if the path end with a slash, indicating that
 * the basefile should be a directory.
 */
int end_with_slash(char *path) {
	int c = 0;
	while (path[c] != '\0') {
		if (path[c+1] == '\0' && path[c] == '/')
			return 1;
		c++;
	}
	return 0;
}


//=========================my system calls==================================
void * my_calloc(size_t num, size_t size) {
	void *result = calloc(num, size);
	if (result==NULL)
	{
		printf ("Error allocating requested memory");
		exit (1);
	}
	return result;
}

void my_close(int fd) {
	if (close(fd))
	{
		perror("file closing not successful.");
		exit(1);	
	}
}


//=======================Helper for cp=======================
/* Check if the source file is a directory */
int src_not_dir(char * path){
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	if S_ISDIR(statbuf.st_mode)
		return 1;
	return 0;
}

/* Get the source file size */
int src_file_size(char * filename){
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

/* copy every block from native file to the target file in disk */
void copy_each_block(int native_file_size,int native_fd,
	int block_count, int * target_blocks){
	int offset = 0, remain = native_file_size, curblock,i;
	unsigned char * native_map = mmap(NULL, native_file_size,
		PROT_READ, MAP_PRIVATE, native_fd, 0);
	if(block_count > 13){
		// handle the indirect block for the copy file here
		int indirect = target_blocks[12];
		int* indirect_block = (void*)disk + EXT2_BLOCK_SIZE * (indirect + 1);
		for(i=13;i<block_count;i++){
			*indirect_block = target_blocks[i] + 1;
			indirect_block++;
		}
	}

	for(i=0;i<block_count;i++){
		if(i == 12) continue;
		curblock = target_blocks[i] + 1;
		if(remain < EXT2_BLOCK_SIZE){
			memcpy(disk+EXT2_BLOCK_SIZE* curblock, native_map + offset,remain);
			break;
		}
		else{
			memcpy(disk+EXT2_BLOCK_SIZE* curblock, native_map + offset,EXT2_BLOCK_SIZE);
			offset += EXT2_BLOCK_SIZE;
			remain -= EXT2_BLOCK_SIZE;
		}
	}
}


//=======================Helper for mkdir=======================
/* Initialize a directory, add "." and ".." as well. Update the
 * corresponding inode fields.
 */
void init_dir(int block_num, int inode_num, int parent_inode_num) {
	struct ext2_inode *inode = get_inode_ptr(inode_num);
	struct ext2_inode *p_inode = get_inode_ptr(parent_inode_num);

	int cur_rec_len;

	gd->bg_used_dirs_count++;
	set_block_bitmap(block_num, 1);
	struct ext2_dir_entry  *dir_entry = (struct ext2_dir_entry *)get_block_ptr(block_num);

	cur_rec_len = get_rec_len(strlen("."));
	init_dir_entry(dir_entry, inode_num, cur_rec_len, ".", 'd');
	inode->i_links_count++;

	dir_entry = get_next_entry(dir_entry);

	cur_rec_len = EXT2_BLOCK_SIZE - get_rec_len(strlen("."));
	init_dir_entry(dir_entry, parent_inode_num, cur_rec_len, "..", 'd');
	p_inode->i_links_count++;
}


//=======================Helper for rm=======================
/* The folloing two functions are used together to delete a file
 * specified by the file name file_name
 */
int del_file(int inode_num, char *file_name) {
	struct ext2_inode *inode = get_inode_ptr(inode_num);

	int result;
	int i;
	for (i = 0; i < 12; ++i)
		if (inode->i_block[i] != 0) {
			result = del_file_block(inode->i_block[i], file_name);
			if (result != -1)
				return result;
		}

	return -1;

}

int del_file_block(int block_num, char *file_name) {
	unsigned char *dir_block = get_block_ptr(block_num);
	struct ext2_dir_entry * prev_entry = (struct ext2_dir_entry *) dir_block;
	struct ext2_dir_entry * cur_entry = (struct ext2_dir_entry *) dir_block;
	char entry_name[EXT2_NAME_LEN + 1];

	int is_head = 1;
	while (is_same_block(cur_entry, block_num))
	{
		get_entry_name(entry_name, cur_entry);
		if (!strcmp(entry_name, file_name)) {
			if (is_head)
			{
				cur_entry->inode = 0;
				cur_entry->name_len = 0;
			} else {
				prev_entry->rec_len += cur_entry->rec_len;
			}
			return 1;
		}

		cur_entry = get_next_entry(cur_entry);
		if (is_head)
			is_head = 0;
		else
			prev_entry = get_next_entry(prev_entry);

	}
	return -1;
}

//-------------------------------remove inode-------------------------------
/* Remove a inode, and free spaces used by it */
void remove_file(int inode_num) {
	struct ext2_inode * inode = get_inode_ptr(inode_num);
	time_t current_time = time(NULL);

	int block_count = inode->i_blocks / 2;
	// handle the case when file have indirect block
	if (block_count > 12) block_count--;
	int data_blocks[block_count];
	get_data_block(data_blocks, block_count, inode_num);

	int i;
	if (get_inode_type(inode_num) == 'f')
	{
		inode->i_links_count--;
		if (inode->i_links_count == 0)
		{
			inode->i_dtime = (unsigned int)current_time;
			for (i = 0; i < block_count; i++)
				set_block_bitmap(data_blocks[i], 0);
			set_block_bitmap(inode->i_block[12], 0);
			set_inode_bitmap(inode_num, 0);
		}
	} else if (get_inode_type(inode_num) == 's')
	{
		inode->i_dtime = (unsigned int)current_time;
		inode->i_links_count--;
		for (i = 0; i < block_count; i++)
			set_block_bitmap(data_blocks[i], 0);

		set_inode_bitmap(inode_num, 0);
	}
}

//========================Helper for restore========================
/* The folloing two functions are used to search directory, 
 * with gap searching 
 */
int search_dir_restore(int inode_num, char *file_name) {
	struct ext2_inode *inode = get_inode_ptr(inode_num);

	int result;
	int i;
	for (i = 0; i < 12; ++i)
		if (inode->i_block[i] != 0) {
			result = search_block_restore(inode->i_block[i], file_name);
			if (result != -1)
				return result;
		}
	return -1;
}

int search_block_restore(int block_num, char *file_name) {
	unsigned char *dir_block = get_block_ptr(block_num);

	struct ext2_dir_entry * prev_entry = (struct ext2_dir_entry *) dir_block;
	struct ext2_dir_entry * cur_entry = (struct ext2_dir_entry *) dir_block;
	struct ext2_dir_entry * next_entry = (struct ext2_dir_entry *) dir_block;

	char dir_entry_name[EXT2_NAME_LEN + 1];
	while (is_same_block(cur_entry, block_num))
	{
		get_entry_name(dir_entry_name, cur_entry);
		if (!strcmp(dir_entry_name, file_name)) {
			if (get_inode_bm_bit(cur_entry->inode)) {
				perror("Inode already in use");
				exit(ENOENT);
			} else {
				if (try_restore_inode(cur_entry->inode) == -1)
					break;

				if (get_dir_entry_type(cur_entry->file_type) != 'd')
				{
					set_inode_bitmap(cur_entry->inode, 1);
					next_entry = find_prev_entry(block_num, &prev_entry, cur_entry);
					prev_entry->rec_len = (void *)cur_entry - (void *)prev_entry;
					cur_entry->rec_len = (void *)next_entry - (void *)cur_entry;
					return cur_entry->inode;
				} else {
					perror("Is a directory");
					exit(EISDIR);
				}
			}
		}

		cur_entry = (struct ext2_dir_entry *)((void *)cur_entry + 
			get_rec_len(cur_entry->name_len));

	}
	return -1;
}

/* Get the previous allocated entry, i.e., the entry that is not in gap */
struct ext2_dir_entry * find_prev_entry(int block_num, struct ext2_dir_entry ** prev_entry, 
		struct ext2_dir_entry * cur_entry) {
	struct ext2_dir_entry * next_entry;
	int prev_rec_len;
	while (is_same_block(*prev_entry, block_num))
	{
		if ((*prev_entry) >= cur_entry)
		{
			*prev_entry = (struct ext2_dir_entry *)((void *)(*prev_entry) - prev_rec_len);
			return next_entry;
		}
		prev_rec_len = (*prev_entry)->rec_len;
		*prev_entry = (struct ext2_dir_entry *)((void *)(*prev_entry) + prev_rec_len);
		next_entry = *prev_entry;
	}
	if ((*prev_entry) >= cur_entry) {
		*prev_entry = (struct ext2_dir_entry *)((void *)(*prev_entry) - prev_rec_len);
	}
	return next_entry;
}

/* Attemp to restore the inode */
int try_restore_inode(int inode_num) {
	struct ext2_inode * inode = get_inode_ptr(inode_num);
	if (inode->i_links_count > 0) {
		return -1;
	}

	int block_count = inode->i_blocks / 2;
	// Indirect block
	if (block_count > 12) block_count--;
	int data_blocks[block_count];
	get_data_block(data_blocks, block_count, inode_num);
	int i;
	for (i = 0; i < block_count; ++i)
	{
		if (data_blocks[i] == 0) continue;

		if (get_block_bm_bit(data_blocks[i]) ||
			(i == 12 && get_block_bm_bit(inode->i_block[i])))
		{
			perror("Some block(s) is already used");
			exit(ENOENT);
		}
	}

	for (i = 0; i < block_count; ++i)
	{
		if (data_blocks[i] == 0) continue;
		set_block_bitmap(data_blocks[i], 1);
		if (i == 12)
			set_block_bitmap(inode->i_block[i], 1);
	}
	inode->i_links_count++;
	inode->i_dtime = 0;

	return 1;
}
