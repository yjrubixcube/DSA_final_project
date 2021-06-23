#include "api.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct trie_node{
    struct trie_node *child[36];
    int id;  //index in disjoint set array
    bool is_name;
} trie_node;
trie_node *trie_root; //trie for group_analyse

typedef struct stack
{
    struct stack *next;
    int type; //0:(, 1:), 2:not, 3:and, 4:or, 5:bool, 6:token
    bool exp;
    char *token;
}stack;

stack* push(stack *stk, int type, bool exp, char *w){
    stack *new = (stack*)malloc(sizeof(stack));
    new->type = type;
    new->exp = (type==5) ? exp : false;
    new->token = w;
    new->next = stk;
    return new;
}

stack* pop(stack *stk){

}

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<36; i++) new->child[i] = NULL;
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
        //cur->id = string_num;
        //string_num += 1;
    }

    return cur->id; 

}

//this will insert a token into the trie
void insert_token(trie_node *root, char *w){
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
}

bool check_token(trie_node *root, char *w, int length){
    trie_node *cur = root;
    int level = 0;
    int index;
    bool flag = true;

    while (level<length && flag){
        if (w[level]>='a'){
            index = w[level]-'a';
        }
        else if (w[level]>='A'){
            index = w[level]-'A';
        }
        else if (w[level]>='0'){
            index = w[level]-'0';
        }

        if (cur->child[index] == NULL){
            flag = false;
        }
        else{
            cur = cur->child[index];
            level++;
        }
    }
    
    //if the token dne
    if (!cur->is_name){
        flag = false;
    }

    return flag;
}

//this adds all the tokens in the expression into trie
//TODO
void preprocess(char *exp, trie_node *root, stack *stk){
    int index = 0;
    while(exp[index]!='\0'){
        int length=0;
        while ((exp[index+length]<='z' && exp[index+length]>='A')|| (exp[index+length] <= '9' && exp[index+length] >= '0')){
            length+=1;
            
        }
        if (length>0){
            char ins[length], temp = exp[index+length];
            exp[index+length]='\0';
            strcpy(ins, exp+index);
            insert_token(root, ins);
            exp[index+length]=temp;
            index+=length;
        }else{
            index+=1;
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
        if(queries[i].type == expression_match){

        }
    }
    
    return 0;
}
