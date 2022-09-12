#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"

/* 10007 */
#define TABLE_SIZE 3
#define BRANCH_SIZE 5


/* STRUCTS */

/* tree_node (tlink):
 * (nodes for the n-ary tree)
 *  - char* path / char* value:
 *		pointer to chars storing a a path and its value, in each node
------------------------------------------------------------------
 *  - int n_children:
 *		number of children the node has, allways greater or equal to 0
------------------------------------------------------------------
 *  - tlink** children:
 *		array of pointers to different tree nodes
------------------------------------------------------------------ */

struct tree_node
{	
	char* path;
	char* value;
	int n_children;
	int len;
	struct tree_node** children;
};


/* n-ary tree methods */


/* Innitializes the N-Ary tree and returns it's root */
tlink* tree_init()
{
	int i;
	tlink* root = (struct tree_node*) malloc(sizeof(struct tree_node));

	root->path = NULL;
	root->value = NULL;
	root->n_children = 0;
	root->len = BRANCH_SIZE;

	/* gives the children array a starting length of 5 and innitializes each
	   index as NULL */
	root->children = (struct tree_node**) malloc(sizeof(struct tree_node*) * BRANCH_SIZE);

	for(i = 0; i < BRANCH_SIZE; i++) {
		root->children[i] = NULL;
	}

	return root;
}


/* creates a new tree node and returns ot */
tlink* create_tlink(char* p, char* v)
{
	int i;
	tlink* new_node = (struct tree_node*) malloc(sizeof(struct tree_node));

	new_node->path = NULL;
	new_node->value = NULL;
	new_node->n_children = 0;
	new_node->len = BRANCH_SIZE;
	
	new_node->children = (struct tree_node**) malloc(sizeof(struct tree_node*) * BRANCH_SIZE);

	for(i = 0; i < BRANCH_SIZE; i++) {
		new_node->children[i] = NULL;
	}

	if(p != NULL) {
		new_node->path = (char*) malloc(sizeof(char) * (strlen(p) + 1));
		strcpy(new_node->path, p);
	}

	if(v != NULL) {
		new_node->value = (char*) malloc(sizeof(char) * (strlen(v) + 1));
		strcpy(new_node->value, v);
	}

	return new_node;
}



void add_tlink(tlink* root, tlink* new_node)
{
	int i = 0;
	// TODO: think about this --> tlink* new_node = create_tlink(p, v);
	
	/* returns the first index where root->children is null */
	for(i = 0; i < root->len; i++) {
		if(root->children[i] == NULL)
			break;
	}

	/* if i has reached the end of the children and none were NULL
	   we'll alloc more space */
	if(i == root->len-1) {
		root->len *= 2;
		root->children = (struct tree_node**) realloc(root->children, sizeof(struct tree_node*) * root->len);

		i++;

		root->children[i] = (struct tree_node*) malloc(sizeof(struct tree_node));
		root->children[i] = new_node;

		i++;

		for(; i < root->len; i++) {
			root->children[i] = NULL;
		}
	}
	else {
		root->children[i] = (struct tree_node*) malloc(sizeof(struct tree_node));
		root->children[i] = new_node;
	}

	root->n_children += 1;
}


tlink* find_tlink_value(tlink* tree_node, char* value)
{
	int i;
	for(i = 0; i < tree_node->len; i++) {
		if(tree_node->children[i] == NULL)
			continue;
		else
			if(!strcmp(tree_node->children[i]->value, value))
				return tree_node->children[i];
	}

	return NULL;
}


tlink* get_tlink(tlink* root, char* path, char* value)
{
	int i;

	if(!strcmp(root->path, path)) {
		
		if(!strcmp(root->value, value))
			return root;

		find_tlink_value(root, value);
	}

	for(i = 0; i < root->len; i++) {
		if(root->children[i] == NULL)
			continue;
		else
			get_tlink(root->children[i], path, value);
	}
}



void free_tlink(tlink* tree_node)
{
	int i;
	free(tree_node->path);
	free(tree_node->value);

	for(i = 0; i < tree_node->len; i++) {

		if(tree_node->children[i] != NULL)
			free_tlink(tree_node->children[i]);
	}

	free(tree_node->children);
	free(tree_node);
}


void delete_tlink(tlink* father_node, tlink* tree_node)
{
	free_tlink(tree_node);
	father_node->n_children -= 1;
}


void free_tree(tlink* tree_root)
{
	free_tlink(tree_root);
}


void print_tree(tlink* tree_root)
{
	int i;

	printf("**********\npath: %s\nvalue: %s\n", tree_root->path, tree_root->value);
	for(i = 0; i < tree_root->len; i++) {
		if(tree_root->children[i] == NULL)
			continue;
		else
			print_tree(tree_root->children[i]);
	}
}

/* hash_node (hlink):
/* (nodes for the hashtable)
    
/*  - char* path / char* value:
/*		pointer to chars storing a a path and its value, in each node
------------------------------------------------------------------
/*  - hlink* next:
/*		pointer to the next node in the same index
------------------------------------------------------------------
/*  - hlink* prev:
/*		pointer to the previous node in the same index
------------------------------------------------------------------
/*  - tlink* tnode:
/*		pointer to a tree node
------------------------------------------------------------------ */

struct hash_node
{
	char* path;
	char* value;
	struct hash_node* next;
	struct hash_node* prev;
	struct tree_node* tnode;
};


/* hashtable methods */

/* Innitializes the hashtable and returns it */
hlink** hash_innit()
{	
	int i;
	hlink** Htable = (struct hash_node**) malloc(sizeof(struct hash_node*) * TABLE_SIZE);

	for(i = 0; i < TABLE_SIZE; ++i)
		Htable[i] = NULL;

	return Htable;
}



/* hashing funtion. Returns the hastable key for the desired path */
int hash(char* path)
{
	long int hash, a = 31415, b = 27183;

	for(hash = 0; *path != 0; path++, a = a*b%(TABLE_SIZE-1))
		hash = (a*hash + *path) % TABLE_SIZE;

	return hash;
}



/* creates a node for the hashtable, containg "p" as its path and "v" as its value */
hlink* create_hlink(char* p, char* v, tlink* tree_node)
{
	hlink* new_node = (struct hash_node*) malloc(sizeof(struct hash_node));
	new_node->path = NULL;
	new_node->value = NULL;

	if(p != NULL) {
		new_node->path = (char*) malloc(sizeof(char) * (strlen(p) + 1));
		strcpy(new_node->path, p);
	}

	if(v != NULL) {
		new_node->value = (char*) malloc(sizeof(char) * (strlen(v) + 1));
		strcpy(new_node->value, v);
	}

	new_node->next = NULL;
	new_node->prev = NULL;
	new_node->tnode = tree_node;

	return new_node;
}


/* returns the last hash node from a certain position in the hastable, essentially gets
   the last node of the "linked list" that is each position of the hashtable */
hlink* get_last_node(hlink* head)
{
	while(head->next != NULL)
		head = head->next;

	return head;
}



/* adds a node to the hastable */
void add_hlink(hlink** Htable, char* p, char* v, tlink* tree_node)
{
	int key;
	hlink* new_head = NULL;

	key = hash(p);
	hlink* new_node = create_hlink(p, v, tree_node);

	hlink* head = Htable[key];

	if(head == NULL)
		Htable[key] = new_node;

	else {
		new_head = get_last_node(head);

		new_head->next = new_node;
		new_node->prev = new_head;
	}
}



/* finds the hash node with the same value as the one given, if it doesnt exist, returns NULL */
hlink* get_hlink(hlink** Htable, char* p, char* v)
{
	int key;

	key = hash(p);
	hlink* head = Htable[key];

	if(head == NULL)
		return NULL;

	while(head->next != NULL) {

		if(!strcmp(head->value, v))
			return head;

		head = head->next;
	}

	return NULL;
}



/* frees all the memory from the hash node */
void free_hlink(hlink* head)
{
	free(head->path);
	free(head->value);
	free(head);
}



/* deletes a node from the hashtable */
void delete_hlink(hlink** Htable, hlink* head)
{
	int key;
	key = hash(head->path);

	/* the only node in that position */
	if(head->prev == NULL && head->next == NULL) {
		free_hlink(head);
		Htable[key] = NULL;
	}

	/* a node in the middle of the list */
	else if(head->prev != NULL && head->next != NULL) {
		head->prev->next = head->next;
		head->next->prev = head->prev;
		free_hlink(head);
	}

	/* first node of the list */
	else if(head->prev == NULL && head->next != NULL) {
		head->next->prev = NULL;
		Htable[key] = head->next;
		free_hlink(head);
	}

	/* last node of the list */
	else if(head->prev != NULL && head->next == NULL) {
		head->prev->next = NULL;
		free_hlink(head);
	}
}



/* deletes all the hash nodes in that hashtable position */
void free_list(hlink** Htable, int key)
{
	while(Htable[key] != NULL)
		delete_hlink(Htable, Htable[key]);

	free(Htable[key]);
}



/* frees the hashtable and all the memory associated with it */
void free_hashtable(hlink** Htable)
{
	int i;

	for(i = 0; i < TABLE_SIZE; ++i)
		free_list(Htable, i);

	free(Htable);
}


void print_list(hlink* head)
{
	while(head != NULL) {
		printf("path: %s --> value: %s\n", head->path, head->value);
		head = head->next;
	}
}


void print_table(hlink** Htable)
{
	int i;

	for(i = 0; i < TABLE_SIZE; ++i) {
		printf("--------\nkey: %d\n", i);
		print_list(Htable[i]);
	}
}



int main()
{
	char p1[3] = "a/";
	char v1[3] = "oi";
	char p2[4] = "a/b";
	char v2[4] = "ola";
	char v3[4] = ">:)";

	hlink** table = hash_innit();
	tlink* tree_root = tree_init();

	
	tlink* one = create_tlink(p1, v1);
	tlink* two = create_tlink(p2, v2);
	tlink* three = create_tlink(p2, v3);

	add_tlink(tree_root, one);
	//add_tlink(one, two);
	//add_tlink(one, three);
	
	printf("path: %s\nsize: %d\n-----\n", tree_root->children[0]->path, tree_root->n_children);
	//printf("path: %s\nsize: %d\n-----\n", one->children[0]->path, one->n_children);
	//printf("path: %s\nsize: %d\n-----\n", one->children[1]->path, one->n_children);

	add_hlink(table, p1, v1, NULL);
	add_hlink(table, p2, v2, NULL);
	add_hlink(table, p1, v3, NULL);

	//print_tree(tree_root);
	print_table(table);

	free_tree(tree_root);
	free_hashtable(table);

	return 0;
}