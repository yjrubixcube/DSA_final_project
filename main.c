#include "api.h"
#include <stdio.h>

// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;

//TODO: make a hash

typedef struct dset
{
	dsnode *head, *tail;
	int size;
}dset;

typedef struct dsnode{
	dset *parent;
	struct dsnode *next;
	//some info
}dsnode;

dset *find_set(dsnode *node){

}

void Union(dset *s1, dset *s2){ //by size
	if ()
}

int main(void) {
	api.init(&n_mails, &n_queries, &mails, &queries);

	for(int i = 0; i < n_queries; i++)
		if(queries[i].type == expression_match)
		  	api.answer(queries[i].id, NULL, 0);

  	return 0;
}
