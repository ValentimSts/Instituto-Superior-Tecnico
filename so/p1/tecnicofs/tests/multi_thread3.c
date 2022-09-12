#include "fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>

void *test(void *args) {
    char *dest_path = (char *)args;

    tfs_copy_to_external_fs("/f1", dest_path);

    return NULL;
}

/* runs tfs_copy_to_external_fs 3 times, copying the contents from the same file */

int main() {

    int f;
    ssize_t r; 
    char file[4] = "/f1";
    char *str = "JV Third MultiThread test: concurent tfs_copy_to_external_fs() calls";

    char *dest_path1 = "newFile1";
    char *dest_path2 = "newFile2";
    char *dest_path3 = "newFile3";

    pthread_t threads[3];

    assert(tfs_init() != -1);

    /* Creates a file */
    f = tfs_open(file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    assert(pthread_create(&threads[0], NULL, test, dest_path1) == 0);
    assert(pthread_create(&threads[1], NULL, test, dest_path2) == 0);
    assert(pthread_create(&threads[2], NULL, test, dest_path3) == 0);

    assert(pthread_join(threads[0], NULL) == 0);
    assert(pthread_join(threads[1], NULL) == 0);
    assert(pthread_join(threads[2], NULL) == 0);

    assert(tfs_destroy() != -1);

    printf("Test Successful!\n");
    return 0;
}