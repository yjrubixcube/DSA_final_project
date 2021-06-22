#include "api.h"
#include <stdio.h>
// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.
#define HASH_SIZE 100

int n_mails, n_queries;
mail *mails;
query *queries;

typedef struct dset
{
	dsnode *head, *tail;
	int size;
}dset;

typedef struct dsnode{
	dset *parent;
	struct dsnode *next;
	char *name;
	//some info
}dsnode;

dsnode *hash_table[HASH_SIZE]={NULL};

dsnode *find_node(char *str){
	unsigned long h = hash(str);
	while (hash_table[h]!=NULL){
		if (hash_table[h]->name!=str){
			h++; //need better probing method
		}
		else{
			return hash_table[h];
		}
	}
}

dset *find_set(char *str){
	dsnode *node = find_node(str);
	return node->parent;

}

void init_node(char *str){
	dsnode *new = (dsnode *)malloc(sizeof(dsnode));
	new->name = str;
	new->next = NULL;
	new->parent = NULL; //what should this be
}

unsigned long hash(char *str){
	// google: dbj2
	unsigned long h = 5381;
	int c;
	while (c=*str++){
		h = ((h<<5)+h)+c; //h*33+c
	}
	return h%HASH_SIZE;
}

void Union(dset *s1, dset *s2){ //by size
}

int main(void) {
	api.init(&n_mails, &n_queries, &mails, &queries);

	for(int i = 0; i < n_queries; i++)
		if(queries[i].type == expression_match)
		  	api.answer(queries[i].id, NULL, 0);

  	return 0;
}
