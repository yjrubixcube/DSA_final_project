//Trie for searching the id of the name

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int trie_size = 0;

typedef struct trie_node{
    struct trie_node *child[26];
    int id;  //index in disjoint set array
    bool is_word;
} trie_node;

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<26; i++) new->child[i] = NULL;
    new->is_word = false;
    new->id = -1;

    return new;
} 

//find name id
//and insert the name in Trie if it doesn't exist 
int get_name_id(trie_node *root, char *w){
    trie_node *cur = root;
    int level = 0;
    int index;

    while(w[level] != '\0'){
        //first character is in uppercase
        index = w[level] - ((level>0) ? 'a' : 'A');
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

int main(){
    trie_node *root = build_node();
    
    //for testing
    while(1){
        char name[11];
        scanf("%s", name);
        printf("%d\n", get_name_id(root, name));
    }
    //show(root, 0);
    return 0;
}


