/*
 * DO NOT MODIFY
 *
 * This file makes it easy to compile an ext2cat binary against the reference
 * implementation and is used by the test suite.
 */

#include "ext2fs.h"
#include "reference_implementation.h"
#include "ext2_access.h"


struct ext2_super_block * get_super_block(void * fs) {
    return _ref_get_super_block(fs);
}

__u32 get_block_size(void * fs) {
    return _ref_get_block_size(fs);
}

void * get_block(void * fs, __u32 block_num) {
    return _ref_get_block(fs, block_num);
}

struct ext2_group_desc * get_block_group(void * fs, __u32 block_group_num) {
    return _ref_get_block_group(fs, block_group_num);
}

struct ext2_inode * get_inode(void * fs, __u32 inode_num) {
    return _ref_get_inode(fs, inode_num);
}

char ** split_path(char * path) {
    int num_slashes = 0;
    for (char * slash = path; slash != NULL; slash = strchr(slash + 1, '/')) {
        num_slashes++;
    }

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

struct ext2_inode * get_root_dir(void * fs) {
    return get_inode(fs, EXT2_ROOT_INO);
}

__u32 get_inode_from_dir(void * fs, struct ext2_inode * dir, 
        char * name) {
    return _ref_get_inode_from_dir(fs, dir, name);
}

__u32 get_inode_by_path(void * fs, char * path) {
    return _ref_get_inode_by_path(fs, path);
}

