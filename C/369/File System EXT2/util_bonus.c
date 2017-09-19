#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "util_bonus.h"
#include <errno.h>



extern unsigned char* disk;
extern struct ext2_super_block *sb;
extern struct ext2_group_desc *gd;
extern unsigned char *inode_bitmap,  *block_bitmap;
extern char *inode_table;
extern int *block_bits,*inode_bits;



/* initialize a new pathNode in the pathLink */
path* pathNode(char * name, char type){
    path * cur = (path*) calloc(1,sizeof(path));
    cur->name = (char *) calloc(strlen(name)+1,sizeof(char));
    strncpy(cur->name,name,strlen(name));
    cur->type = type;
    return cur;
}


/* make a linklist of the path */
path * pathLink(char * disk_path){
    char curname[LineMax];
    char cur;
    int i,pathlen = strlen(disk_path);
    path * head = pathNode(".",'d');
    path * curNode = head;
    if(pathlen == 1)
        return head;
    if(pathlen == 2 && disk_path[0] == '.'){
        if(disk_path[1] != '/'){
            return NULL;
        }
        return head;
    }


    memset(curname,'\0',LineMax);
    for(i=1;i<pathlen;i++){
        cur = disk_path[i];
        if(cur == '/' && i== 1)
            continue;
        else if(cur == '/'){
            path * newNode = pathNode(curname,'d');
            curNode->next = newNode;
            curNode = newNode;
            memset(curname,'\0',LineMax);
            continue;
        }
        strncat(curname,&disk_path[i],sizeof(char));
        if(i == pathlen -1){
            path * newNode = pathNode(curname,'t');
            curNode->next = newNode;
            curNode = newNode;
            memset(curname,'\0',LineMax);
        }
    }

    curNode->next = NULL;
    return head;
}



/*  Obtain the file name from the path
*   /a/b/c -> c
*   ./a/b/c -> c
*/
char * filenameObtain(char * native_path){
    char result[LineMax];
    memset(result,'\0',LineMax);
    int i,cur=0;
    char check;
    for(i=strlen(native_path);i>0;i--){
        check = native_path[i-1];
        if(i == strlen(native_path) && check == '/'){
            perror("must be a specific file");
            exit(ENOENT);
        }
        else if(check == '/')
            break;
        else{
            result[cur] = check;
            cur++;
        }
    }
    cur = 0;
    char * rval = (char *)calloc(1,strlen(result)+1);
    for(i=strlen(result);i>0;i--){
        rval[cur] = result[i-1];
        cur++;
    }
    return rval;
}





/* free the path link allocated*/
void freepath(path * head){
    path * cur = head;
    while(cur != NULL){
        head = head->next;
        free(cur->name);
        free(cur);
        cur = head;
    }
}

/* set the bit of the offset position of the block_bitmap to val*/
void set_inode_bitmap(int offset, int val){
    if(inode_bits[offset] == val)
        return;
    int byte = offset / 8;
    int bit = offset % 8;
    unsigned char * target = inode_bitmap + byte;
    *target ^= (-val ^ *target) & (1 << bit);
    gd->bg_free_inodes_count += 1 - 2*val;
    sb->s_free_inodes_count += 1-2*val;
    inode_bits[offset] = val;
}

/* set the bit of the offset position of the inode_bitmap to val*/
void set_block_bitmap(int offset,int val){
    if(block_bits[offset] == val)
        return;
    int byte = offset / 8;
    int bit = offset % 8;
    unsigned char * target = block_bitmap + byte;
    *target ^= (-val ^ *target) & (1 << bit);
    gd->bg_free_blocks_count += 1- 2*val;
    sb->s_free_blocks_count += 1 - 2*val;
    block_bits[offset] = val;
}


/*
* return the inode index of the last directory
* return -1 if not found
*/
int path_walk(path * mypath){
    struct ext2_dir_entry *dir;
    struct ext2_inode *curnode;
    int n, block_count,i,j,k;
    char check,filename[LineMax];
    path * cur = mypath;
    i = EXT2_ROOT_INO;
    int find = 0;
    while(cur != NULL){
        if( !inode_bits[i-1]){
            perror("not valid inode in the path");
            exit(ENOENT);
        }
        find = 0;
        curnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * (i-1));
        if(curnode->i_mode & EXT2_S_IFDIR){
            block_count = curnode->i_blocks / 2;

            for (j = 0; j < block_count; j++){
              k = 0;
              while (k < EXT2_BLOCK_SIZE){
                dir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * curnode->i_block[j] + k);
                k += dir->rec_len;

                memset(filename,'\0',sizeof(filename));
                for(n = 0; n < dir->name_len; n++){
                	check = dir->name[n];
                	if((check == '.' && (n != 0 && n!= dir->name_len -1) )|| check == '\0')
                		break;
                    strncat(filename,&dir->name[n],1);
                }
                if(strlen(filename) == strlen(cur->name) &&(strncmp(filename,cur->name,strlen(filename)) == 0)){
                    find = 1;
                    cur = cur->next;
                    i = dir->inode;
                    break;
                }
                // if end of the path but the file doesn't exists
                if(cur->type == 't'){
                    find = 1;
                    cur = cur->next;
                    break;
                }

              }
              if( find == 1){
                  break;
              }
            }
        }
        if( find == 0){
            return -1;
        }
    }
    return i - 1;
}





// align for string and integer
int alignname(char * name){return (((strlen(name)-1) / 4) + 1 ) * 4;}
int alignint(int namelen){return (((namelen-1)/4) + 1) * 4;}







/* initialize :
*  (1) the disk (shared)
*  (2) the superblock (shared)
*  (3) the ext2_group_desc (shared)
*  (4) the inode_bitmap and the block_bitmap (shared)
*  (5) the inode_bits and block_bits (private)
*/
void initialize(int diskfd){
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, diskfd, 0);
    if (disk == MAP_FAILED) {
      perror("mmap");
      exit(1);
    }
    sb = (struct ext2_super_block *)(disk + 1024);
    gd = (struct ext2_group_desc *)(disk + 1024 * 2);
    // get the inode_bitmap and the block_bitmap and the inode table
    inode_bitmap = ((EXT2_BLOCK_SIZE * gd->bg_inode_bitmap) + disk);
    block_bitmap = ((EXT2_BLOCK_SIZE * gd->bg_block_bitmap) + disk);
    inode_table = (char *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table));

    block_bits = (int *)calloc(sb->s_blocks_count,sizeof(int));
    inode_bits = (int *)calloc(sb->s_inodes_count,sizeof(int));
    int i,j,cur;
    for (i = 0; i < (sb->s_blocks_count / 8); i++) {
      for (j = 0; j < 8; j++) {
        cur = (*(block_bitmap + i) >> j) & 1;
        block_bits[(i * 8) + j] = cur;
      }
    }
    for (i = 0; i < (sb->s_inodes_count / 8); i++) {
      for (j = 0; j < 8; j++) {
        cur = (*(inode_bitmap + i) >> j) & 1;
        inode_bits[(i * 8) + j] = cur;
      }
    }
}




// free the block bit and inode bit
void freeAll(){
    free(block_bits);
    free(inode_bits);
}


/* get the block that contains data ie. exclude the indirect block*/
void get_data_block(int *container,int block_count,int inodeidx){
    int *indirect_block,i;
    struct ext2_inode * parend_inode =
      (struct ext2_inode *)(inode_table + sb->s_inode_size * inodeidx);
    for(i = 0;i < block_count;i++){
        if(i<12){
            container[i] = parend_inode->i_block[i];
        }
        if(i== 12){
            indirect_block = (void *)disk + EXT2_BLOCK_SIZE * (parend_inode->i_block[12]);
        }
        else if(i > 12){
            container[i-1] = *indirect_block;
            indirect_block++;
        }
    }
}


// ================= you need ===================

/*  Remove the file entry from the directory
*   decrease the link and unset the inode and blocks if linkcount is 0
*
*   (1) if the direntry in the first entry in the block,
*       set the dir->inode to be zero and clear the name
*   (2) resie the rec_len otherwise
*
*/
int remove_from_directory(int parent_inodedx, char * native_file_name){
    struct ext2_inode * parend_inode =
      (struct ext2_inode *)(inode_table + sb->s_inode_size * parent_inodedx);
    int d_len = 0,i;
    struct ext2_dir_entry * pdir,*prevdir = NULL;
    int pblock_count = parend_inode->i_blocks / 2;
    // handle the case when dir have indirect block
    int have_indirect = (pblock_count > 12) ? 1 : 0 ;
    int data_block_count = (have_indirect == 1) ? pblock_count-1 : pblock_count;
    int data_block[data_block_count];

    get_data_block(data_block,data_block_count,parent_inodedx);

    for(i = 0;i<pblock_count;i++){
        prevdir = NULL;
        pdir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * data_block[i]);
        d_len = 0;
        while(d_len<EXT2_BLOCK_SIZE){
            d_len += pdir->rec_len;
            if(strlen(native_file_name) == pdir->name_len){
                if(strncmp(native_file_name,pdir->name,pdir->name_len) == 0){
                    if(pdir->rec_len == d_len){
                        pdir->inode = 0;
                        pdir->name_len = 0;
                        pdir->name[0] = '\0';
                        break;
                    }
                    prevdir->rec_len += pdir->rec_len;
                    break;
                }
            }
            prevdir = pdir;
            pdir = (void *)pdir + pdir->rec_len;
        }
    }

    return 0;
}






/*  restore the file entry from the directory
*   increase the link and reset the inode and blocks if the linkcount is 1
*
*   (1) if the direntry in the first entry in the block,
*       unable to restore
*   (2) if the inode or any block of the inode is used, unable to restore
*   (3) return 0 is success
*/
int restore_to_directory(int parent_inodedx, char * native_file_name,int restoreDir){
    struct ext2_inode * parend_inode =
      (struct ext2_inode *)(inode_table + sb->s_inode_size * parent_inodedx);
    int d_len = 0,find = 0,i,need;
    struct ext2_dir_entry * pdir;
    int pblock_count = parend_inode->i_blocks / 2;

    // handle the case when dir have indirect block


    int have_indirect = (pblock_count > 12) ? 1 : 0 ;
    int data_block_count = (have_indirect == 1) ? pblock_count-1 : pblock_count;
    int data_block[data_block_count];




    get_data_block(data_block,data_block_count,parent_inodedx);



    for(i = 0;i<pblock_count;i++){
        pdir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * data_block[i]);
        d_len = 0;
        while(d_len<EXT2_BLOCK_SIZE){
            d_len += pdir->rec_len;

            need = alignint(pdir->name_len) + sizeof(struct ext2_dir_entry);
            if(need + sizeof(struct ext2_dir_entry) < pdir->rec_len){
                // search all inner block

                int curlen = need;
                int same = curlen;
                struct ext2_dir_entry * dir = (void*)pdir + need;
                while(curlen < pdir->rec_len){
                    curlen += dir->rec_len;
                    if(curlen == same){
                        // garbage file
                        break;
                    }

                    //
                    if(dir->name_len > 0){
                        int l;
                        for(l=0;l<dir->name_len;l++){
                        }
                    }

                    //


                    if(strlen(native_file_name) == dir->name_len){
                        if(strncmp(native_file_name,dir->name,dir->name_len) == 0){

                            if(isDir(dir->inode-1)){
                                if(restoreDir == 0)
                                    return -1;
                                else{
                                    parend_inode->i_links_count++;
                                }

                            }

                            int check = check_restoreable(dir);
                            if(check == -1){
                                perror("inode used, cant recover");
                                exit(ENOMEM);
                            }
                            if(check == -2){
                                perror("Unable to recover in this situation");
                                exit(ENOENT);
                            }
                            if(check == -3){
                                perror("block in use, can't recover");
                                exit(ENOMEM);
                            }

                            curlen -= dir->rec_len;
                            int newlen = pdir->rec_len - curlen;
                            pdir->rec_len = curlen;
                            dir->rec_len = newlen;
                            if(isDir(dir->inode-1) == 0)
                                increaseLink(dir->inode-1);
                            return dir->inode -1;
                        }
                    }


                    dir = (void *)dir + dir->rec_len;
                    same = curlen;
                }
            }
            pdir = (void *)pdir + pdir->rec_len;
        }
    }


    if(find == 0){
        perror("no such file");
        exit(ENOENT);
    }

    return 0;
}


// ======================= HELPER FUNCTION FOR CP ============================

/* check whether the file name already exist under the parent inode */
int check_existence(char * checkname,int parent_inodedx){
    struct ext2_dir_entry *dir;
    int j,block_count,cur,n;
    char filename[256];
    struct ext2_inode * curnode;
    curnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * parent_inodedx);
    block_count = curnode->i_blocks / 2;

    for (j = 0; j < block_count; j++){
      cur = 0;
      while (cur < EXT2_BLOCK_SIZE){
        dir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * curnode->i_block[j] + cur);
        cur += dir->rec_len;
        memset(filename,'\0',LineMax);
        for(n = 0; n < dir->name_len; n++){
          strncat(filename,&dir->name[n],1);
        }
        if(strcmp(filename,checkname) == 0){
            return dir->inode-1;
        }
      }
    }
    return 0;
}

/* check whether the file in the disk path is a dir entry*/
int isDir(int inodeindex){
    struct ext2_inode * curnode;
    curnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * inodeindex);
    if(curnode->i_mode & EXT2_S_IFDIR){
        return 1;
    }
    return 0;
}

// ================= helper function of Bonus part ===============

/* recursively remove a direntry*/
int remove_dir(int index){
    struct ext2_inode * srcnode;
    srcnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * index);

    int d_len = 0,i;
    struct ext2_dir_entry * dir;
    int block_count = srcnode->i_blocks / 2;
    // handle the case when dir have indirect block
    int have_indirect = (block_count > 12) ? 1 : 0 ;
    int data_block_count = (have_indirect == 1) ? block_count-1 : block_count;
    int data_block[data_block_count];

    get_data_block(data_block,data_block_count,index);
    for(i = 0;i<block_count;i++){
        dir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * data_block[i]);
        d_len = 0;
        while(d_len<EXT2_BLOCK_SIZE){
            d_len += dir->rec_len;
            if(isDir(dir->inode-1)){
                if(is_filename(dir) && dir->inode - 1 != index){
                    gd->bg_used_dirs_count--;
                    remove_dir(dir->inode -1);
                }
            }
            else{
                unset_inode(dir->inode-1);
            }
            dir = (void *)dir + dir->rec_len;
        }
    }

    unset_inode(index);

    return 0;

}

/* set the inode bit map to be zero and all its block to be zero if the link count is zero*/
void unset_inode(int index){
    struct ext2_inode * srcnode;
    srcnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * index);

    int block_count = srcnode->i_blocks/2;
    int *indirect_block,i,cur;

    // check it is in hard link with other
    if(!(srcnode->i_mode & EXT2_S_IFDIR)){
        srcnode->i_links_count--;
        if(srcnode->i_links_count > 0){
            return;
        }
    }
    srcnode->i_dtime = (unsigned int)time(NULL);


    set_inode_bitmap(index,0);
    for(i = 0;i < block_count;i++){
        if(i<12){
            cur = srcnode->i_block[i];
            set_block_bitmap(cur-1,0);
        }
        if(i== 12){
            indirect_block = (void *)disk + EXT2_BLOCK_SIZE * (srcnode->i_block[12]);
            cur = srcnode->i_block[12];
            set_block_bitmap(cur-1,0);
        }
        else if(i > 12){
            set_block_bitmap(*indirect_block-1,0);
            indirect_block++;
        }
    }

}

/* if the name is "." or ".." return 0, else return 1*/
int is_filename(struct ext2_dir_entry * dir){
    char name[LineMax];
    memset(name,'\0',LineMax);
    int i;
    for(i=0;i<dir->name_len;i++){
        name[i] = dir->name[i];
    }
    if(strlen(name) == 1 && strncmp(name,".",1) == 0){
        return 0;
    }
    if(strlen(name) == 2 && strncmp(name,"..",2) == 0){
        return 0;
    }
    return 1;
}

/* increase the linkcount of the inode at this index*/
void increaseLink(int index){
    struct ext2_inode * srcnode;
    srcnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * index);
    srcnode->i_links_count++;
}


// ======================== restore bonus
/* recursively restore a direntry*/
int restore_dir(int index){
    struct ext2_inode * srcnode =
      (struct ext2_inode *)(inode_table + sb->s_inode_size * index);

      int d_len = 0,i;
      struct ext2_dir_entry * pdir,*prevdir;
      int pblock_count = srcnode->i_blocks / 2;
      // handle the case when dir have indirect block
      int have_indirect = (pblock_count > 12) ? 1 : 0 ;
      int data_block_count = (have_indirect == 1) ? pblock_count-1 : pblock_count;
      int data_block[data_block_count];

      get_data_block(data_block,data_block_count,index);


      for(i = 0;i<pblock_count;i++){
          pdir = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * data_block[i]);
          prevdir = NULL;
          d_len = 0;

          while(d_len<EXT2_BLOCK_SIZE){
              d_len += pdir->rec_len;
              if(check_restoreable(pdir) == 0){
                  // dir
                  if((isDir(pdir->inode-1)) && pdir->inode-1 != index){
                      restore_dir(pdir->inode - 1);
                      if(is_filename(pdir)){
                          gd->bg_used_dirs_count++;
                      }
                  }
                  else{
                      reset_inode(pdir->inode - 1);
                  }
                  prevdir = pdir;
              }
              else{
                  if(prevdir == NULL){
                      return -1;
                  }
                  else{
                      // update the reclen
                      prevdir->rec_len += pdir->rec_len;
                  }

              }


              pdir = (void *)pdir + pdir->rec_len;
          }
      }
      reset_inode(index);



      return 0;
}

/* check whether the file or direntry is able to be restored
 * if any of the inode is in use of any block of its inode is in use
 * return negative value, else return 0
 */
int check_restoreable(struct ext2_dir_entry * dir){
    if(isDir(dir->inode-1)){ // change
        if(dir->name_len == 2 && strncmp("..",dir->name,2)==0){
            return 0;
        }
    }

    if(inode_bits[dir->inode-1]){
        return -1; // inode in use
    }
    if(dir->inode == 0){
        return -2; // unable to restore
    }

    struct ext2_inode * r =
        (struct ext2_inode*)(inode_table +sb->s_inode_size * (dir->inode-1) );
    int icount = r->i_blocks / 2;
    int k = 0;
    for(k=0;k<icount;k++){
        if(block_bits[r->i_block[k]-1] == 1){
            return -3; // block in use
        }
    }
    return 0;
}



/* set the inode bitmap of the inode at this index to be 1
 * set all its block in the bitmao to be one
 */
void reset_inode(int index){
    struct ext2_inode * srcnode;
    srcnode = (struct ext2_inode *)(inode_table + sb->s_inode_size * index);

    int block_count = srcnode->i_blocks/2;
    int *indirect_block,i,cur;

    // check it is in hard link with other
    if(!(srcnode->i_mode & EXT2_S_IFDIR)){
        srcnode->i_links_count++;
        if(srcnode->i_links_count > 1){
            return;
        }
    }

    if(inode_bits[index] == 0){
        set_inode_bitmap(index,1);
    }



    for(i = 0;i < block_count;i++){
        if(i<12){
            cur = srcnode->i_block[i];
            if(block_bits[cur-1] == 0)
                set_block_bitmap(cur-1,1);
        }
        if(i== 12){
            indirect_block = (void *)disk + EXT2_BLOCK_SIZE * (srcnode->i_block[12]);
            cur = srcnode->i_block[12];
            if(block_bits[cur-1] == 0)
                set_block_bitmap(cur-1,1);
        }
        else if(i > 12){
            if(block_bits[*indirect_block-1] == 0)
                set_block_bitmap(cur-1,1);
            indirect_block++;
        }
    }

}
