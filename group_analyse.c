#include "api.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int dset_size = 0;  //number of group in disjoint set
int trie_size = 0;  //number of names stored in the trie 

typedef struct dset{
    int size;
    int parent;
} dset;
dset ds[600];

void dset_init(dset *ds){
    for(int i=0; i<600; i++){
        ds[i].parent = i;
        ds[i].size = 1;
    }
    dset_size = 0;
}

typedef struct trie_node{
    struct trie_node *child[26];
    int id;  //index in disjoint set array
    bool is_word;
} trie_node;
trie_node *trie_root; //trie for group_analyse

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<26; i++) new->child[i] = NULL;
    new->is_word = false;
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
    if(!cur->is_word){
        cur->is_word = true;
        cur->id = trie_size;
        trie_size += 1;
        dset_size += 1;;
    }

    return cur->id; 

}

//show content of the Trie
void show(trie_node *root, int level){
    level += 1;
    for(int i=0; i<26; i++){
        if(root->child[i] != NULL){
            for(int i=1; i<level; i++) printf("-");
            printf("%c", i+'a');
            if(root->child[i]->is_word) printf(" name id: %d", root->child[i]->id);
            printf("\n");

            show(root->child[i], level);
        }
    }
}

int get_root(int i){
    //path compression
    if(ds[i].parent != i) ds[i].parent = get_root(ds[i].parent);
    return ds[i].parent;
}

int find_set(char *name){
    int id = get_name_id(trie_root, name);

    return get_root(id);
}

//haven't finished
void Union(char *n1, char *n2){
    int id1 = find_set(n1);
    int id2 = find_set(n2);
    ds[id1].parent = ds[id2].parent;
}


// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;


int main(){
	api.init(&n_mails, &n_queries, &mails, &queries);
    
    trie_root = build_node(); //initialize root

	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == group_analyse){
            printf("-----------group_analyse query-----------\n");
            int n = queries[i].data.group_analyse_data.len;
            int *ids = queries[i].data.group_analyse_data.mids;
            dset_init(ds);

            for(int j=0; j<n; j++){
                mail t_mail = mails[ids[j]];
            
                char *from = t_mail.from;
                char *to = t_mail.to;
                //printf("From:%s\n", from);
                //printf("To:%s\n", to);
                Union(from, to);
            }
        
        }
    }
    
    return 0;
}
