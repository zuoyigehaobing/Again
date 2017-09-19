#include "ext2.h"
#include "ext2_utils.h"

#define MAX_STR_LEN 2000

int main(int argc, char **argv) {

	if(argc != 4) {
		fprintf(stderr, "Usage: %s <image file name> <source path> <destination path>\n",
		 argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	// Source file checking.
	int native_fd = open(argv[2],O_RDONLY);
	if(native_fd == -1){
		perror("error opening file\n");
		exit(ENOENT);
	}

	if(src_not_dir(argv[2])){
		char rval[MAX_STR_LEN];
		sprintf(rval,"omitting directory %s ",argv[2]);
		perror(rval);
		exit(EISDIR);
	}

	if (argv[3][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	int file_size = get_src_file_size(argv[2]);
	int block_count = blocks_needed(file_size);

	if (sb->s_free_blocks_count < block_count)
	{
		perror("No enough blocks");
		exit(ENOMEM);
	}

	// Extract path.
	Linkedlist * disk_path = path_link(argv[3]);

	// Search disk.
	char target_name[EXT2_NAME_LEN + 1];
	memset(target_name, '\0', sizeof(target_name));
	int end_slash = end_with_slash(argv[3]);
	int dir_inode = find_path_inode(disk_path, argv[2], end_slash, target_name);

	// Initialize inode. Add directory entry.
	if (sb->s_free_inodes_count < 1) {
		perror("No enough space for new inode");
		exit(ENOMEM);
	} 

	int new_inode_num = get_free_inode_num();
	add_dir_entry(dir_inode, new_inode_num, 'f', target_name, block_count);
	set_inode_bitmap(new_inode_num, 1);

	// Write data.
	int target_blocks[block_count];
	get_free_blocks(target_blocks, block_count);
	init_inode(new_inode_num, EXT2_S_IFREG,file_size,target_blocks);
	copy_each_block(file_size, native_fd, block_count, target_blocks);

	// Destory data structures.
	free_llist(disk_path);
	my_close(fd);
	my_close(native_fd);

	return 0;
}
