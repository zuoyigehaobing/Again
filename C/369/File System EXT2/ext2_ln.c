#include "ext2.h"
#include "ext2_utils.h"

int main(int argc, char **argv) {

	if(!(argc == 4 || (argc == 5 && !strcmp(argv[2], "-s")))) {
		fprintf(stderr, "Usage: %s <image file name> <-s> <source> <destination>\n", 
			argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	int arg_src = 2;
	int arg_dest = 3;

	if (argc == 5)
	{
		arg_src++;
		arg_dest++;
	}
		
	if (argv[arg_src][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	if (argv[arg_dest][0] != '/')
	{
		perror("Absolute path expected");
		exit(1);
	}

	Linkedlist * src = path_link(argv[arg_src]);
	Linkedlist * dest = path_link(argv[arg_dest]);

	int end_slash = end_with_slash(argv[arg_src]);
	int src_inode = find_file_inode(src, argc == 5, end_slash);
	char target_name[EXT2_NAME_LEN + 1];
	memset(target_name, '\0', sizeof(target_name));
	int parent_inode_num = find_dir_inode(dest, target_name);

	if (argc == 4)
	{
		struct ext2_inode * inode = get_inode_ptr(src_inode);
		inode->i_links_count++;
		add_dir_entry(parent_inode_num, src_inode, 'f', target_name, 0);

	} else {
		if (argc == 5) {
			if (sb->s_free_inodes_count < 1)
			{
				perror("No enough space for new inode");
				exit(ENOMEM);
			}

			if (sb->s_free_blocks_count < 1)
			{
				perror("No enough blocks");
				exit(ENOMEM);
			}

			int empty_inode_num = get_free_inode_num();
			add_dir_entry(parent_inode_num, empty_inode_num, 's', target_name, 1);
			set_inode_bitmap(empty_inode_num, 1);

			int target_blocks[1];
			get_free_blocks(target_blocks, 1);
			init_inode(empty_inode_num, EXT2_S_IFLNK, 
				strlen(argv[arg_src]), target_blocks);

			char * block = (char *)get_block_ptr(target_blocks[0] + 1);
			strncpy(block, argv[arg_src], EXT2_BLOCK_SIZE);

		} else {
			perror("No such option");
			exit(1);
		}
	}

	my_close(fd);
	free_llist(src);
	free_llist(dest);

	return 0;
}
