#ifndef __EXT2_ACCESS_H
#define __EXT2_ACCESS_H

#include "ext2fs.h"


struct ext2_super_block * get_super_block(void * fs);

__u32 get_block_size(void * fs);

void * get_block(void * fs, __u32 block_num);

struct ext2_group_desc * get_block_group(void * fs, __u32 block_group_num);

struct ext2_inode * get_inode(void * fs, __u32 inode_num);

char ** split_path(char * path);

struct ext2_inode * get_root_dir(void * fs);

__u32 get_inode_from_dir(void * fs, struct ext2_inode * dir, char * name);

__u32 get_inode_by_path(void * fs, char * path);

#endif

