#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2.h"
#include "ext2_utils.h"

void check_bm();
void check_dir(int dir_inode_num, int *colour);
int check_dir_block(int block_num, int *colour);

int fix_count;

int main(int argc, char **argv) {

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <image file name>\n",
		 argv[0]);
		exit(1);
	}

	int fd = init_disk_ptrs(argv[1]);

	fix_count = 0;
	int colour[sb->s_inodes_count];
	int i;
	for (i = 0; i < sb->s_inodes_count; i++)
		colour[i] = 0;

	check_bm();

	check_dir(2, colour);

	if (fix_count > 0)
		printf("%d file system inconsistencies repaired!\n", fix_count);
	else
		printf("No file system inconsistencies detected!\n");

	my_close(fd);

	return 0;
}

void check_bm() {
	int i;
	int inode_free_count = sb->s_inodes_count;
	int block_free_count = sb->s_blocks_count;
	
	// inode bitmap
	for (i = 1; i <= sb->s_inodes_count; i++) {
		inode_free_count -= !!get_inode_bm_bit(i);
	}

	if (inode_free_count != sb->s_free_inodes_count) {
		printf("Fixed: superblock's %s counter was off by %d compared to the bitmap\n", 
			"free inodes", abs(sb->s_free_inodes_count - inode_free_count));
		fix_count += abs(sb->s_free_inodes_count - inode_free_count);
		sb->s_free_inodes_count = inode_free_count;
	}
	
	if (inode_free_count != gd->bg_free_inodes_count) {
		printf("Fixed: block group's %s counter was off by %d compared to the bitmap\n", 
			"free inodes", abs(gd->bg_free_inodes_count - inode_free_count));
		fix_count += abs(gd->bg_free_inodes_count - inode_free_count);
		gd->bg_free_inodes_count = inode_free_count;
	}

	// block bitmap
	for (i = 1; i <= sb->s_blocks_count; i++) {
		block_free_count -= !!get_block_bm_bit(i);
	}

	if (block_free_count != sb->s_free_blocks_count) {
		printf("Fixed: superblock's %s counter was off by %d compared to the bitmap\n", 
			"free blocks", abs(sb->s_free_blocks_count - block_free_count));
		fix_count += abs(sb->s_free_blocks_count - block_free_count);
		sb->s_free_blocks_count = block_free_count;
	}

	if (block_free_count != gd->bg_free_blocks_count) {
		printf("Fixed: block group's %s counter was off by %d compared to the bitmap\n", 
			"free blocks", abs(gd->bg_free_blocks_count - block_free_count));
		fix_count += abs(gd->bg_free_blocks_count - block_free_count);
		gd->bg_free_blocks_count = block_free_count;
	}	
}

void check_dir(int dir_inode_num, int *colour) {
	struct ext2_inode *inode = get_inode_ptr(dir_inode_num);	
	colour[dir_inode_num - 1] = 1;

	if (get_inode_bm_bit(dir_inode_num) == 0)
	{
		set_inode_bitmap(dir_inode_num, 1);
		fix_count++;

		printf("Fixed: inode [%d] not marked as in-use\n", dir_inode_num);
	}

	struct ext2_inode * inode_ptr = get_inode_ptr(dir_inode_num);
	if (inode_ptr->i_dtime != 0)
	{
		inode_ptr->i_dtime = 0;
		fix_count++;

		printf("Fixed: valid inode marked for deletion: [%d]\n", dir_inode_num);
	}
	
	int block_count = inode->i_blocks / 2;
	// handle the case when file have indirect block
	if (block_count > 12) block_count--;
	int data_blocks[block_count];
	get_data_block(data_blocks, block_count, dir_inode_num);

	int i;
	for (i = 0; i < block_count; ++i)
		if (inode->i_block[i] != 0) {
			if (i == 12)
			{
				if (get_block_bm_bit(inode->i_block[i]) == 0) {
					set_block_bitmap(inode->i_block[i], 1);
					fix_count++;

					printf("Fixed: %d in-use data blocks not marked in data bitmap for inode: [%d]\n", 
						inode->i_block[i], dir_inode_num);
				}
			}

			if (get_block_bm_bit(data_blocks[i]) == 0) {
				set_block_bitmap(data_blocks[i], 1);
				fix_count++;

				printf("Fixed: %d in-use data blocks not marked in data bitmap for inode: [%d]\n", 
					data_blocks[i], dir_inode_num);
			}

			if (get_inode_type(dir_inode_num) == 'd') {
				check_dir_block(inode->i_block[i], colour);
			}
		}
		else
			break;
}

int check_dir_block(int block_num, int *colour) {
	unsigned char *dir_block = get_block_ptr(block_num);
	struct ext2_dir_entry * cur_entry = (struct ext2_dir_entry *) dir_block;

	while ((unsigned char *)cur_entry < dir_block + EXT2_BLOCK_SIZE && cur_entry->inode != 0)
	{
		if (get_inode_type(cur_entry->inode) != get_dir_entry_type(cur_entry->file_type)) {
			cur_entry->file_type = convert_file_type(get_inode_type(cur_entry->inode));
			fix_count++;

			printf("Fixed: Entry type vs inode mismatch: inode [%d]\n", cur_entry->inode);
		}

		if (colour[cur_entry->inode - 1] == 0)
			check_dir(cur_entry->inode, colour);

		cur_entry = (struct ext2_dir_entry *)((unsigned char *)cur_entry + cur_entry->rec_len);
	}
	return -1;
}
