#include "ext2.h"
#include "ext2_utils.h"

int main(int argc, char **argv) {

	if(argc != 3) {
		fprintf(stderr, "Usage: %s <image file name> <restore file>\n",
		 argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	if (argv[2][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	Linkedlist * llist = path_link(argv[2]);

	char target_name[EXT2_NAME_LEN + 1];
	memset(target_name, '\0', sizeof(target_name));
	int parent_inode_num = find_dir_inode(llist, target_name);

	int try = search_dir_restore(parent_inode_num, target_name);
	if (try == -1)
	{
		perror("Cannot restore file");
		exit(1);
	}

	my_close(fd);
	free_llist(llist);

	return 0;
}
