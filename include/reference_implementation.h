#include "ext2fs.h"


/*
 * Accessors for the basic components of ext2.
 */
// Return a pointer to the primary superblock of a filesystem.
struct ext2_super_block * _ref_get_super_block(void * fs);


// Return the block size for a filesystem.
__u32 _ref_get_block_size(void * fs);


// Return a pointer to a block given its number.
// get_block(fs, 0) == fs;
void * _ref_get_block(void * fs, __u32 block_num);


// Return a pointer to the first block group descriptor in a filesystem. Real
// ext2 filesystems will have several of these, but, for simplicity, we will
// assume there is only one.
struct ext2_group_desc * _ref_get_block_group(void * fs, __u32 block_group_num);


// Return a pointer to an inode given its number. In a real filesystem, this
// would require finding the correct block group, rather than assuming it's in
// the first.
struct ext2_inode * _ref_get_inode(void * fs, __u32 inode_num);



/*
 * High-level code for accessing files by path.
 */
// Chunk a filename into pieces.
// split_path("/a/b/c") will return {"a", "b", "c"}.
char ** _ref_split_path(char * path);


// Convenience function to get the inode of the root directory.
struct ext2_inode * _ref_get_root_dir(void * fs);


// Given the inode for a directory and a filename, return the inode number of
// that file inside that directory, or 0 if it doesn't exist.
// 
// name should be a single component - "foo.txt", not "/files/foo.txt".
__u32 _ref_get_inode_from_dir(void * fs, struct ext2_inode * dir, 
        char * name);


// Top-level function to find the inode number for a file by its full path.
__u32 _ref_get_inode_by_path(void * fs, char * path);

