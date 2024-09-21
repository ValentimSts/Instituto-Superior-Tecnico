#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

typedef struct hash_map_entry hentry;
typedef struct hash_node hnode;

struct hash_map {
    int size;
    hentry** entries;
};

struct hash_map_entry {
    int size;
    hnode* head;
    hnode* tail;
};

struct hash_node {
    int index;
    data* d;
    hnode* next;
    hnode* prev;
};


static void print_map(hmap* hm);
static hentry* create_entry();
static int destroy_entry(hentry* he);
static int insert_entry(hentry* he, data* d);
static int update_entry(hentry* he, data* d);
static int remove_from_entry(hentry* he, data* d);
static data* search_entry(hentry* he, data* d);
static hnode* create_hash_node();
static void link_hash_nodes(hnode* hn1, hnode* hn2);
static int update_node_indexes(hnode* removed_hn);
static int destroy_hash_node(hnode* hn);
static int insert_node(hnode* hn, data* d);
static int update_node(hnode* hn, data* d);
static int remove_node(hnode* hn);
static hnode* search_node(hnode* hn, data* d);


hmap* create_hash_map() {
    hmap* hm = (hmap*) malloc(sizeof(hmap));
    if (hm == NULL) {
        return NULL;
    }

    hm->size = TABLE_SIZE;
    hm->entries = (hentry**) malloc(sizeof(hentry*) * hm->size);
    if (hm->entries == NULL) {
        free(hm);
        return NULL;
    }

    for (int i = 0; i < hm->size; i++) {
        hentry* he = create_entry();
        if (he == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_entry(hm->entries[j]);
            }

            free(hm->entries);
            free(hm);

            return NULL;
        }

        hm->entries[i] = he;
    }

    return hm;
}

int destroy_hash_map(hmap* hm) {
    if (hm == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < hm->size; i++) {
        if (destroy_entry(hm->entries[i]) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    free(hm->entries);
    free(hm);

    return EXIT_SUCCESS;
}

int insert_into_hash_map(hmap* hm, data* d) {
    if (hm == NULL || d == NULL) {
        return EXIT_FAILURE;
    }

    int index = hash(d, hm->size);
    return insert_entry(hm->entries[index], d);
}

int update_hash_map(hmap* hm, data* d) {
    if (hm == NULL || d == NULL) {
        return EXIT_FAILURE;
    }

    int index = hash(d, hm->size);
    return update_entry(hm->entries[index], d);
}

int remove_from_hash_map(hmap* hm, data* d) {
    if (hm == NULL || d == NULL) {
        return EXIT_FAILURE;
    }

    int index = hash(d, hm->size);
    return remove_from_entry(hm->entries[index], d);
}

data* search_hash_map(hmap* hm, data* d) {
    if (hm == NULL || d == NULL) {
        return NULL;
    }

    int index = hash(d, hm->size);
    return search_entry(hm->entries[index], d);
}

static void print_map(hmap* hm) {
    for (int i = 0; i < hm->size; i++) {
        printf("Entry %d:\n", i);
        hentry* he = hm->entries[i];
        hnode* hn = he->head;
        for (int j = 0; j < he->size; j++) {
            printf("    Node %d: %s\n", j, hn->d->value);
            hn = hn->next;
        }
    }
}



static hentry* create_entry() {
    hentry* he = (hentry*) malloc(sizeof(hentry));
    if (he == NULL) {
        return NULL;
    }

    he->size = 0;
    he->head = NULL;
    he->tail = NULL;

    return he;
}

static int destroy_entry(hentry* he) {
    if (he == NULL) {
        return EXIT_FAILURE;
    }

    hnode* hn = he->head;
    for (int i = 0; i < he->size; i++) {
        hnode* tmp = hn;
        hn = hn->next;

        if (destroy_hash_node(tmp) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    free(he);

    return EXIT_SUCCESS;
}

static int insert_entry(hentry* he, data* d) {
    hnode* hn = create_hash_node();
    if (hn == NULL) {
        return EXIT_FAILURE;
    }

    int entry_size = he->size;

    hn->index = entry_size;

    if (entry_size == 0) {
        he->head = hn;
        he->tail = hn;
    } else {
        link_hash_nodes(he->tail, hn);
        he->tail = hn;
    }

    he->size++;

    return insert_node(hn, d);
}

static int update_entry(hentry* he, data* d) {
    hnode* hn = search_node(he->head, d);
    if (hn == NULL) {
        return EXIT_FAILURE;
    }

    return update_node(hn, d);
}

static int remove_from_entry(hentry* he, data* d) {
    hnode* hn = search_node(he->head, d);
    if (hn == NULL) {
        return EXIT_FAILURE;
    }

    int removed_node_index = hn->index;
    if (removed_node_index == -1) return EXIT_FAILURE;

    int entry_size = he->size;

    if ((entry_size == 1 && removed_node_index == 0) ||
        (removed_node_index == entry_size - 1)) {
        // Only one node in the entry
        he->head = NULL;
        he->tail = NULL;
    }
    else if (removed_node_index == 0) {
        // More than one node in the entry, and the
        // first node is being removed
        if (update_node_indexes(hn) != EXIT_SUCCESS) return EXIT_FAILURE;
        he->head = hn->next;
    } 
    else if (removed_node_index == entry_size - 1) {
        // More than one node in the entry, and the
        // last node is being removed
        he->tail = hn->prev;
    }
    else {
        // More than one node in the entry, and a node
        // in the middle is being removed
        if (update_node_indexes(hn) != EXIT_SUCCESS) return EXIT_FAILURE;
        link_hash_nodes(hn->prev, hn->next);
    }

    he->size--;

    if (destroy_hash_node(hn) != EXIT_SUCCESS) return EXIT_FAILURE;

    return remove_node(hn);
}

static data* search_entry(hentry* he, data* d) {
    if (he->size == 0) return NULL;

    hnode* hn = search_node(he->head, d);
    if (hn == NULL) return NULL;

    return hn->d;
}



static hnode* create_hash_node() {
    hnode* hn = (hnode*) malloc(sizeof(hnode));
    if (hn == NULL) return NULL;

    hn->index = -1;
    hn->d = NULL;
    hn->next = NULL;
    hn->prev = NULL;

    return hn;
}

static void link_hash_nodes(hnode* hn1, hnode* hn2) {
    hn1->next = hn2;
    hn2->prev = hn1;
}

static int update_node_indexes(hnode* removed_hn) {
    if (removed_hn == NULL) return EXIT_FAILURE;

    hnode* hn = removed_hn->next;
    while (hn != NULL) {
        hn->index--;
        hn = hn->next;
    }

    return EXIT_SUCCESS;
}

static int destroy_hash_node(hnode* hn) {
    if (hn == NULL) {
        return EXIT_FAILURE;
    }

    if (destroy_data(hn->d) != EXIT_SUCCESS) return EXIT_FAILURE;
    
    free(hn);

    return EXIT_SUCCESS;
}

static int insert_node(hnode* hn, data* d) {
    hn->d = d;
    return EXIT_SUCCESS;
}

static int update_node(hnode* hn, data* d) {
    hn->d = d;
    return EXIT_SUCCESS;
}

static int remove_node(hnode* hn) {
    return destroy_hash_node(hn);
}

static hnode* search_node(hnode* hn, data* d) {
    while (hn != NULL) {
        if (compare_data(hn->d, d) == 0) return hn;
        hn = hn->next;
    }

    return NULL;
}


int main() {
    hmap* hm = create_hash_map();
    if (hm == NULL) {
        return EXIT_FAILURE;
    }

    data* d1 = create_data("\a", "data1");
    data* d2 = create_data("\a\b", "data2");
    data* d3 = create_data("\a\b\\c", "data3");

    insert_into_hash_map(hm, d1);
    insert_into_hash_map(hm, d2);
    insert_into_hash_map(hm, d3);

    print_map(hm);

    printf("\n");

    data* d4 = create_data("\a", "data4");
    update_hash_map(hm, d4);

    print_map(hm);

    printf("\n");

    data* d5 = create_data("\a\b", "data5");
    remove_from_hash_map(hm, d5);

    print_map(hm);

    destroy_hash_map(hm);

    return EXIT_SUCCESS;
}