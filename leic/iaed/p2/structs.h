#ifndef _STRUCTS_H_
#define _STRUCTS_H_


/* Tree struct / methods */

typedef struct tree_node tlink;


/* HashTable struct / methods */

typedef struct hash_node hlink;

hlink** hash_innit();
int hash(char* path);
hlink* create_hlink(char* p, char* v, tlink* tree_node);
hlink* get_last_node(hlink* head);
void add_hlink(hlink** Htable, char* p, char*v, tlink* tree_node);
hlink* get_hlink(hlink** Htable, char* p, char* v);
void free_hlink(hlink* head);
void delete_hlink(hlink** Htable, hlink* head);
void free_list(hlink** Htable, int key);
void free_hashtable(hlink** Htable);

void print_list(hlink* head);
void print_table(hlink** Htable);


#endif