#include "api.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int group_num = 0;  //number of group in disjoint set
int string_num = 0;  //number of names stored in the trie 
int max_group_size = 0;

typedef struct dset{
    int size;
    int parent;
} dset;
dset ds[600];

void dset_init(dset *ds){
    for(int i=0; i<600; i++){
        ds[i].parent = -1;
        ds[i].size = 0;
    }
    group_num = 0;
    max_group_size = 0;
}

typedef struct trie_node{
    struct trie_node *child[26];
    int id;  //index in disjoint set array
    bool is_name;
} trie_node;
trie_node *trie_root; //trie for group_analyse

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<26; i++) new->child[i] = NULL;
    new->is_name = false;
    new->id = -1;

    return new;
} 

//this will return the index of the name in the disjoint set
int get_name_id(trie_node *root, char *w){
    trie_node *cur = root;
    int level = 0;
    int index;

    while(w[level] != '\0'){
        //first character is in uppercase
        index = w[level] - ((w[level]>='a') ? 'a' : 'A');
        if(cur->child[index] == NULL){
            cur->child[index] = build_node();
        }
        cur = cur->child[index];
        level += 1;
    }
    
    //the name doesn't exist
    if(!cur->is_name){
        cur->is_name = true;
        cur->id = string_num;
        string_num += 1;
    }

    return cur->id; 

}

void make_set(int i){
    ds[i].parent = i;
    ds[i].size = 1;
}

int get_root(int i){
    int i0 = i;
	//path compression
    while(ds[i].parent != i)
		i = ds[i].parent;
		
	int temp;
	while(i0 != i){
		temp = ds[i0].parent;
		ds[i0].parent = i;
		i0 = temp;
	}
    
	return i;
}

int find_set(char *name){
    int id = get_name_id(trie_root, name);
    if(ds[id].size == 0) make_set(id);

    return get_root(id);
}

void Union(char *n1, char *n2){
    int id1 = find_set(n1);
    int id2 = find_set(n2);
    
    if(id1 != id2)
	{
		if(ds[id1].size > 1 && ds[id2].size > 1)
			group_num--;
		else if(ds[id1].size == 1 && ds[id2].size == 1)
			group_num++;
		
		if(ds[id1].size >= ds[id2].size){
	    	ds[id2].parent = id1;
	    	ds[id1].size += ds[id2].size;
	    	
	    	if(ds[id1].size > max_group_size)
	    		max_group_size = ds[id1].size;
		}
		else{
			ds[id1].parent = id2;
	    	ds[id2].size += ds[id1].size;
	    	
	    	if(ds[id2].size > max_group_size)
	    		max_group_size = ds[id2].size;
		}
	}
}


// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;


int main(){
	api.init(&n_mails, &n_queries, &mails, &queries);
    
    trie_root = build_node(); //initialize root

	mail *t_mail;
	int n;
	int *ids;
	int ans[2];
	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == group_analyse){
            n = queries[i].data.group_analyse_data.len;
            ids = queries[i].data.group_analyse_data.mids;
            dset_init(ds);

            for(int j=0; j<n; j++){
                t_mail = &mails[ids[j]];
                Union(t_mail->from, t_mail->to);
            }
            ans[0] = group_num;
            ans[1] = max_group_size;
            api.answer(i, ans, 2);
        }
    }
    
    return 0;
}
