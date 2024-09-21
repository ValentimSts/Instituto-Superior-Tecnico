#ifndef _DATA_H_
#define _DATA_H_

typedef struct data {
    char* path;
    char* value;
} data;

int hash(data* d, int upper_bound);

data* create_data(char* path, char* value);
int destroy_data(data* d);
int update_data(data* d, char* value);
int compare_data(data* d1, data* d2);

#endif // _DATA_H_