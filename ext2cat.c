#include <unistd.h>
#include <stdio.h>

#include "ext2_access.h"
#include "mmapfs.h"


int main(int argc, char ** argv) {
    // Extract the file given in the second argument from the filesystem image
    // given in the first.
    if (argc != 3) {
        printf("usage: ext2cat <filesystem_image> <path_within_filesystem>\n");
        exit(1);
    }
    char * fs_path = argv[1];
    char * file_path = argv[2];
    void * fs = mmap_fs(fs_path);

    // Get the inode for our target file.
    __u32 target_ino_num = get_inode_by_path(fs, file_path);
    if (target_ino_num == 0) {
        printf("cat: %s does not exist\n", file_path);
        return 1;
    }
    struct ext2_inode * target_ino = get_inode(fs, target_ino_num);

    __u32 block_size = get_block_size(fs);
    __u32 size = target_ino->i_size;
    __u32 bytes_read = 0;
    void * buf = calloc(size, 1);

    // Read the file one block at a time. In the real world, there would be a
    // lot of error-handling code here.
    __u32 bytes_left;
    for (int i = 0; i < EXT2_NDIR_BLOCKS; i++) {
        bytes_left = size - bytes_read;
        if (bytes_left == 0) break;
        __u32 bytes_to_read = bytes_left > block_size ? block_size : bytes_left;
        void * block = get_block(fs, target_ino->i_block[i]);
        memcpy(buf + bytes_read, block, bytes_to_read);
        bytes_read += bytes_to_read;
    }

    write(1, buf, bytes_read);
    if (bytes_read < size) {
        printf("%s: file uses indirect blocks. output was truncated!\n",
               argv[0]);
    }

    return 0;
}

