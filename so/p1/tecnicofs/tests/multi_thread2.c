#include "fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>

void *test1(void *args) {

    char buffer[40];
    char file[4] = "/f1";
    int f;
    ssize_t r;

    char *str = (char *)args;

    f = tfs_open(file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    f = tfs_open(file, 0);
    assert(f != -1);

    r = tfs_read(f, buffer, sizeof(buffer) - 1);

    assert(tfs_close(f) != -1);

    buffer[r] = '\0';
    printf("test1: %s\n", buffer);

    return NULL;
}

void *test2(void *args) {

    char buffer[40];
    char file[4] = "/f2";
    int f;
    ssize_t r;

    char *str = (char *)args;

    f = tfs_open(file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    f = tfs_open(file, 0);
    assert(f != -1);

    r = tfs_read(f, buffer, sizeof(buffer) - 1);

    assert(tfs_close(f) != -1);

    buffer[r] = '\0';
    printf("test2: %s\n", buffer);

    return NULL;
}

void *test3(void *args) {

    char buffer[40];
    char file[4] = "/f3";
    int f;
    ssize_t r;

    char *str = (char *)args;

    f = tfs_open(file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    f = tfs_open(file, 0);
    assert(f != -1);

    r = tfs_read(f, buffer, sizeof(buffer) - 1);

    assert(tfs_close(f) != -1);

    buffer[r] = '\0';
    printf("test3: %s\n", buffer);

    return NULL;
}

/* Performs multiple writes on different files at the same time */

int main() {

    char *str1 = "AAA";
    char *str2 = "BBB";
    char *str3 = "CCC";

    pthread_t threads[3];

    assert(tfs_init() != -1);

    assert(pthread_create(&threads[0], NULL, test1, str1) == 0);
    assert(pthread_create(&threads[1], NULL, test2, str2) == 0);
    assert(pthread_create(&threads[2], NULL, test3, str3) == 0);

    assert(pthread_join(threads[0], NULL) == 0);
    assert(pthread_join(threads[1], NULL) == 0);
    assert(pthread_join(threads[2], NULL) == 0);

    assert(tfs_destroy() != -1);

    /* Expected output:
     * - a combination of the following:
     *   test1: AAA
     *   test2: BBB
     *   test3: CCC
     */

    printf("Successful test.\n");
}