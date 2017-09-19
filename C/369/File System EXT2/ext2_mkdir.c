#include "ext2.h"
#include "ext2_utils.h"

int main(int argc, char **argv) {

	if(argc != 3) {
		fprintf(stderr, "Usage: %s <image file name> <destination path>\n",
		 argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	// Sufficient space checking.
	if (sb->s_free_blocks_count < 1) {
		perror("No enough blocks");
		exit(ENOMEM);
	}

	if (sb->s_free_inodes_count < 1) {
		perror("No free inode available");
		exit(1);
	}

	if (argv[2][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	// Extract path to a linked list.
	Linkedlist * disk_path = path_link(argv[2]);
	if (disk_path->size == 0)
	{
		perror("Cannot create directory");
		exit(EEXIST);
	}

	// Find parent directory.
	char target_name[EXT2_NAME_LEN + 1];
	memset(target_name, '\0', sizeof(target_name));
	int parent_inode_num;
	parent_inode_num = find_dir_inode(disk_path, target_name);

	// Add directory entry, and new inode.
	int block_count = 1;
	int new_inode_num = get_free_inode_num();
	add_dir_entry(parent_inode_num, new_inode_num, 'd', target_name, block_count);
	set_inode_bitmap(new_inode_num, 1);

	int target_blocks[block_count];
	get_free_blocks(target_blocks, block_count);

	init_inode(new_inode_num, EXT2_S_IFDIR, EXT2_BLOCK_SIZE, target_blocks);

	// Initialize the directory.
	init_dir(target_blocks[0] + 1, new_inode_num, parent_inode_num);

	// Destory data structures.
	free_llist(disk_path);
	my_close(fd);

	return 0;
}
