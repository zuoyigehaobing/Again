#include "ext2.h"
#include "ext2_utils.h"

int main(int argc, char **argv) {

	if(argc != 3) {
		fprintf(stderr, "Usage: %s <image file name> <target>\n", 
			argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	if (argv[2][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	Linkedlist * target = path_link(argv[2]);

	int end_slash = end_with_slash(argv[2]);
	int target_inode_num;
	char target_file_name[EXT2_NAME_LEN + 1];
	memset(target_file_name, '\0', sizeof(target_file_name));
	int parent_inode_num = find_rm_inode(target, end_slash, 
		&target_inode_num, target_file_name);

	int del_rval = del_file(parent_inode_num, target_file_name);
	if (del_rval > 0)
	{
		remove_file(target_inode_num);
	}

	my_close(fd);
	free_llist(target);

	return 0;
}
