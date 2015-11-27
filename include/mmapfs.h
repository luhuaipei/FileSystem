#ifndef __MMAPFS_H

/*
 * mmap the filesystem at fs_path. This returns a pointer to a region of memory
 * that is backed by the file at fs_path, avoiding all the icky IO you'd
 * otherwise have to do. This project is read-only, and we tell mmap to enforce
 * this - if you try to write to that memory, your program will segfault.
 *
 * It is good practice to explicitly munmap this pointer, but the system will do
 * it automatically when we exit.
 */
void * mmap_fs(char * fs_path);

#define __MMAPFS_H
#endif

