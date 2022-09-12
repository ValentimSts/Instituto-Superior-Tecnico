#ifndef STATE_H
#define STATE_H

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

/*
 * Directory entry
 */
typedef struct {
    char d_name[MAX_FILE_NAME];
    int d_inumber;
} dir_entry_t;

typedef enum { T_FILE, T_DIRECTORY } inode_type;

/*
 * I-node
 */
typedef struct {
    inode_type i_node_type;
    size_t i_size;

    /* array of data blocks, if the inode is associated with a file,
     * i_data_blocks will store up to 10 direct referenced data blocks (their indexes), 
     * if inode is a directory, it will only store 1 data block */
    int i_data_blocks[MAX_FILE_BLOCKS];
    int i_curr_block;

    /* Stores the index of the indirect referenced data block to be used by a file,
     * if necessary */
    int i_indir_block;
    int i_curr_indir;

    /* I-node's lock */
    pthread_rwlock_t i_lock;
} inode_t;

typedef enum { FREE = 0, TAKEN = 1 } allocation_state_t;

/*
 * Open file entry (in open file table)
 */
typedef struct {
    int of_inumber;
    size_t of_offset;

    /* Open file lock */
    pthread_rwlock_t of_lock;
} open_file_entry_t;

#define INDIR_BLOCK_SIZE (BLOCK_SIZE / sizeof(int))
#define MAX_DIR_ENTRIES (BLOCK_SIZE / sizeof(dir_entry_t))

void state_init();
void state_destroy();

int inode_create(inode_type n_type);
int inode_delete(int inumber);
inode_t *inode_get(int inumber);

int clear_dir_entry(int inumber, int sub_inumber);
int add_dir_entry(int inumber, int sub_inumber, char const *sub_name);
int find_in_dir(int inumber, char const *sub_name);

int data_block_alloc();
int data_block_free(int block_number);
void *data_block_get(int block_number);

int add_to_open_file_table(int inumber, size_t offset);
int remove_from_open_file_table(int fhandle);
open_file_entry_t *get_open_file_entry(int fhandle);

/* New methods */

int free_inode_blocks(int inumber);

int inode_rdlock(int inumber);
int inode_wrlock(int inumber);
int inode_unlock(int inumber);

int of_rdlock(int fhandle);
int of_wrlock(int fhandle);
int of_unlock(int fhandle);

#endif // STATE_H
