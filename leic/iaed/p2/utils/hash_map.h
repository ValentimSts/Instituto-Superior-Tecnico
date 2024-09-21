#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_

#include "data.h"

#define TABLE_SIZE 2

typedef struct hash_map hmap;

hmap* create_hash_map();
int destroy_hash_map(hmap* hm);
int insert_into_hash_map(hmap* hm, data* d);
int update_hash_map(hmap* hm, data* d);
int remove_from_hash_map(hmap* hm, data* d);
data* search_hash_map(hmap* hm, data* d);

#endif // _HASH_MAP_H_