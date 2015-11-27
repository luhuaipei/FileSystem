#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


void * mmap_fs(char * fs_path) {
    // stat the fs image. This gives us lots of information in a struct stat,
    // including size, permissions, and ownership.
    struct stat fs_stat;
    memset(&fs_stat, 0, sizeof(struct stat));
    if (stat(fs_path, &fs_stat) != 0) {
        perror("stat");
        exit(1);
    }

    // open and mmap the file. If successful, mapped_fs will be a pointer to a
    // region of memory backed by our filesystem image.
    int fs_fd = open(fs_path, O_RDONLY);
    if (fs_fd < 0) {
        perror("open");
        exit(1);
    }
    void * mapped_fs = mmap(NULL, fs_stat.st_size, PROT_READ, MAP_SHARED, fs_fd,
                            0);
    if (mapped_fs == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fs_fd);   // We no longer need the fd after mmap.

    return mapped_fs;
}

