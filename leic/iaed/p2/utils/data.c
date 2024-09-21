#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

int hash(data* d, int upper_bound)
{
    long int hash, a = 31415, b = 27183;

    for(hash = 0; *d->path != 0; d->path++, a = a*b%(upper_bound-1))
        hash = (a*hash + *d->path) % upper_bound;

    return hash;
}

data* create_data(char* path, char* value)
{
    data* d = (data*) malloc(sizeof(data));
    if(d == NULL) {
        return NULL;
    }

    d->path = (char*) malloc(strlen(path) + 1);
    if(d->path == NULL) {
        free(d);
        return NULL;
    }
    strcpy(d->path, path);

    d->value = (char*) malloc(strlen(value) + 1);
    if(d->value == NULL) {
        free(d->path);
        free(d);
        return NULL;
    }
    strcpy(d->value, value);

    return d;
}

int destroy_data(data* d)
{
    if(d == NULL) {
        return -1;
    }

    free(d->path);
    free(d->value);
    free(d);

    return 0;
}

int update_data(data* d, char* value)
{
    if(d == NULL) {
        return -1;
    }

    free(d->value);
    d->value = (char*) malloc(strlen(value) + 1);
    if(d->value == NULL) {
        return -1;
    }
    strcpy(d->value, value);

    return 0;
}

int compare_data(data* d1, data* d2)
{
    if(d1 == NULL || d2 == NULL) {
        return -1;
    }

    return strcmp(d1->path, d2->path);
}