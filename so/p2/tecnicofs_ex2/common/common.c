#include "common.h"
#include <stdio.h>


/* 
 * Makes sure "write()" actually writes all the bytes the user requested
 * Inputs:
 *  - file descriptor to write to
 *  - source of the data to write
 *  - size of the data
 * Returns: 0 if successful, -1 otherwise
 */
int write_until_success(int fd, void const *source, size_t size) {
    ssize_t wr;
    while ((wr = write(fd, source, size)) != size && errno == EINTR) {
        /* Nothing to do */
    }
    /* If even after the cycle write() hasn't written all the bytes
     * we want, -1 is returned */
    if (wr != size) {
        return -1;
    }
    return 0;
}


/* 
 * Makes sure "read()" actually reads all the bytes the user requested
 * Inputs:
 *  - file descriptor to read from
 *  - destination of the content read
 *  - size of the content
 * Returns: 0 if successful, -1 otherwise
 */
int read_until_success(int fd, void *destination, size_t size) {
    ssize_t offset = 0, rd;
    while ((rd = read(fd, destination + offset, size)) != size || errno == EINTR) {
        if (rd == -1) {
            return -1;
        }
        /* Updates the current offset */
        offset += rd;
    }

    return 0;
}


/*
 * Makes sure "open()" actually opens the pipe given
 * Inputs:
 *  - path to the pipe
 * Returns: file descriptor of the pipe if successful, -1 otherwise
 */
int open_until_success(char const *pipe_path, int oflag) {
    int fd;
    while ((fd = open(pipe_path, oflag)) == -1 && errno == EINTR) {
        /* Nothing to do */
    }
    /* Returns the current fd, if -1 it will be dealt with later */
    return fd;
}


/*
 * Makes sure "close()" actually closes the file given
 * Inputs:
 *  - file descriptor to close
 * Returns: 0 if successful, -1 otherwise
 */
int close_until_success(int fd) {
    int x;
    while ((x = close(fd)) == -1 && errno == EINTR) {
        /* Nothing to do */
    }
    return x;
}