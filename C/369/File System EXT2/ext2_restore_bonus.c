#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2.h"
#include <errno.h>
#include "util_bonus.h"

unsigned char *disk;
struct ext2_super_block *sb;
struct ext2_group_desc *gd;
unsigned char *inode_bitmap,  *block_bitmap;
char *inode_table;
int * block_bits, *inode_bits;


int main(int argc, char **argv) {

  if (argc < 3 ) {
    fprintf(stderr, "Usage: ext2_restore <image file name> <disk path>\n");
    exit(1);
  }

  int fd = open(argv[1], O_RDWR);
  if(fd == -1){
      perror("error opening file\n");
      exit(ENOENT);
  }

  initialize(fd);

  int restore = 0;
  int src_idx = 2;
  if(argc == 4){
      if(strcmp(argv[2],"-r") == 0){
          restore = 1;
          src_idx ++;
      }
      else{
          fprintf(stderr, "Usage: ext2_resotre <image file name>  <optional -r>  <disk path>\n");
          exit(1);
      }
  }


  char * src_name = filenameObtain(argv[src_idx]);
  path * src_disk_path = pathLink(argv[src_idx]);
  if(src_disk_path  == NULL){
      perror("Invalid path");
      exit(ENOENT);
  }


  int src_dir_inode = path_walk(src_disk_path);
  if(src_dir_inode == -1){
      perror("Invalid path");
      exit(ENOENT);
  }

  int src_inode = check_existence(src_name,src_dir_inode);
  if(src_inode > 0){
      perror("File already exist");
      exit(EEXIST);
  }

  if(restore == 0){
      // make sure it is not a dir
      int src_inode = restore_to_directory(src_dir_inode,src_name,0);
      if(src_inode == -1){
          char rval[LineMax];
          sprintf(rval,"Use : [./ext2_resotre_bonus <image name> -r %s] to restore directory ",argv[src_idx]);
          perror(rval);
          exit(EISDIR);
      }
      reset_inode(src_inode);
  }
  else{
      // restore a dir
      int src_inode = restore_to_directory(src_dir_inode,src_name,0);
      if(src_inode == -1)
          src_inode = restore_to_directory(src_dir_inode,src_name,1);
          // used only when restore a dir : update the link
      gd->bg_used_dirs_count++;

      // recursively restore the dir
      restore_dir(src_inode);


      }







  freepath(src_disk_path);
  free(src_name);
  freeAll();
  return 0;
}
