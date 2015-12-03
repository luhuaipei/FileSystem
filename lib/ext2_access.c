// ext2 definitions from the real driver in the Linux kernel.
#include "ext2fs.h"

// This header allows your project to link against the reference library. If you
// complete the entire project, you should be able to remove this directive and
// still compile your code.
//#include "reference_implementation.h"

// Definitions for ext2cat to compile against.
#include "ext2_access.h"



///////////////////////////////////////////////////////////
//  Accessors for the basic components of ext2.
///////////////////////////////////////////////////////////

// Return a pointer to the primary superblock of a filesystem.
struct ext2_super_block * get_super_block(void * fs) {
    // super block is alwasy specified offset from start of file system
    return (struct ext2_super_block *) ((char*)fs + SUPERBLOCK_OFFSET);
}


// Return the block size for a filesystem.
__u32 get_block_size(void * fs) {
    struct ext2_super_block * superBlk = get_super_block(fs);
  //  printf("%p\n",fs);
 //   printf("blk size: %p\n",superBlk);
    return 1024<<superBlk->s_log_block_size;
}


// Return a pointer to a block given its number.
// get_block(fs, 0) == fs;
void * get_block(void * fs, __u32 block_num) {
    return (void*) ((char*)fs + block_num * get_block_size(fs)); 
} 

// Return a pointer to the first block group descriptor in a filesystem. Real
// ext2 filesystems will have several of these, but, for simplicity, we will
// assume there is only one.
struct ext2_group_desc * get_block_group(void * fs, __u32 block_group_num) {
    struct ext2_super_block * superBlock = get_super_block(fs);
    // get pointer to start of block group descriptor table
    __u32 first_blk = superBlock->s_first_data_block+1;
    return (struct ext2_group_desc *) get_block(fs, first_blk);
}


// Return a pointer to an inode given its number. In a real filesystem, this
// would require finding the correct block group, but you may assume it's in the
// first one.
struct ext2_inode * get_inode(void * fs, __u32 inode_num) {
    struct ext2_super_block * superBlock = get_super_block(fs);
    struct ext2_group_desc * blk_group = get_block_group(fs, 0);
    struct ext2_inode * inode_tbl = (struct ext2_inode *) get_block(fs, blk_group->bg_inode_table);
    //inode num start at 1
    return inode_tbl + inode_num -1;
}



///////////////////////////////////////////////////////////
//  High-level code for accessing filesystem components by path.
///////////////////////////////////////////////////////////

// Return number of "/" in given path
// split_path("/a/b/c") will return 3
int get_number_of_slashes(char * path) {
    int num_slashes = 0;
    for (char * slash = path; slash != NULL; slash = strchr(slash + 1, '/')) {
        num_slashes++;
    }
    return num_slashes;
}

// Chunk a filename into pieces.
// split_path("/a/b/c") will return {"a", "b", "c"}.
//
// This one's a freebie.
char ** split_path(char * path) {
    int num_slashes = get_number_of_slashes(path);

    // Copy out each piece by advancing two pointers (piece_start and slash).
    char ** parts = (char **) calloc(num_slashes, sizeof(char *));
    char * piece_start = path + 1;
    int i = 0;
    for (char * slash = strchr(path + 1, '/');
         slash != NULL;
         slash = strchr(slash + 1, '/')) {
        int part_len = slash - piece_start;
        parts[i] = (char *) calloc(part_len + 1, sizeof(char));
        strncpy(parts[i], piece_start, part_len);
        piece_start = slash + 1;
        i++;
    }
    // Get the last piece.
    parts[i] = (char *) calloc(strlen(piece_start) + 1, sizeof(char));
    strncpy(parts[i], piece_start, strlen(piece_start));
    return parts;
}


// Convenience function to get the inode of the root directory.
struct ext2_inode * get_root_dir(void* fs) {
    return get_inode(fs, EXT2_ROOT_INO);
}


// Given the inode for a directory and a filename, return the inode number of
// that file inside that directory, or 0 if it doesn't exist there.
// 
// name should be a single component: "foo.txt", not "/files/foo.txt".
__u32 get_inode_from_dir(void* fs, struct ext2_inode* dir, char* name) {
    //get the block num of directory
    __u32 root_dir = dir->i_block[0];
    struct ext2_dir_entry* dir_entry = (struct ext2_dir_entry*) get_block(fs,root_dir);
    // walk through the linked list to find the file we want
    while (dir_entry->inode != 0) {
        if (strncmp(name,dir_entry->name, strlen(name)) == 0) {
            return dir_entry->inode;
        }
        else {
            //can't find the file
            dir_entry = (struct ext2_dir_entry *)(((char*)dir_entry)+dir_entry->rec_len);
        }
    }
    return -1;
}
// Find the inode number for a file by its full path.
// This is the functionality that ext2cat ultimately needs.
__u32 get_inode_by_path(void * fs, char * path) {
  //split the path
  char** paths = split_path(path);
  // we should serch from the root node.
  __u32 inode_num = EXT2_ROOT_INO;
  //check every entry in the path to find a file
  for (char ** path = paths; *path != NULL; path++) {
    struct ext2_inode * inode = get_inode(fs, inode_num);
    // check if it is a directory
    if(!LINUX_S_ISDIR(inode->i_mode))
        break;
    inode_num = get_inode_from_dir(fs, inode, *path);
    if(inode_num == 0) {
        break;
    } 
  }
  if (inode_num != EXT2_ROOT_INO) {
    //return the inode 
    return inode_num;
  } else {
    return -1;
  }
}