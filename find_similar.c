#include "api.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int string_num = 0;  //number of names stored in the trie 
int token_sets[10000][5000];
int token_sets_len[10000];
int token_check[140000];

typedef struct trie_node{
    struct trie_node *child[36];
    int id;  //index in disjoint set array
    bool is_token;
} trie_node;
trie_node *trie_root;

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<36; i++) new->child[i] = NULL;
    new->is_token = false;
    new->id = -1;

    return new;
} 

//this will return the index of the name in the disjoint set
int get_token_id(trie_node *root, char *w){
    trie_node *cur = root;
    int level = 0;
    int index;

    while(w[level] != '\0'){
        //first character is in uppercase
        if(w[level] <= '9') index = w[level] - '0';
        else index = w[level]+10 - ((w[level]>='a') ? 'a' : 'A');

        if(cur->child[index] == NULL){
            cur->child[index] = build_node();
        }
        cur = cur->child[index];
        level += 1;
    }
    

    //the token doesn't exist
    if(!cur->is_token){
        cur->is_token = true;
        cur->id = string_num;
        string_num += 1;
    }

    return cur->id;
}

//show content of the Trie
void show(trie_node *root, int level){
    level += 1;
    for(int i=0; i<36; i++){
        if(root->child[i] != NULL){
            for(int j=1; j<level; j++) printf("-");
            if(i >= 10) printf("%c", i+'a'-10);
            else printf("%c", i+'0');

            if(root->child[i]->is_token) printf(" id:%d", root->child[i]->id);
            printf("\n");

            show(root->child[i], level);
        }
    }
}

char token[100];
void token_analysis(int mid, char *text, int index, trie_node *root){
    int i = 0;
    int token_id;
    int len;

    while(text[i] != '\0'){
        len = 0;
        while(('0'<=text[i] && text[i]<='9') || ('A'<=text[i] && text[i]<='Z') || ('a'<=text[i] && text[i]<='z')){ 
            token[len] = text[i];
            i += 1;
            len += 1;
        }

        if(len > 0){
            token[len] = '\0';
            token_id = get_token_id(root, token);
            if(token_check[token_id] != mid){
                token_check[token_id] = mid;
                token_sets[mid][index++] = token_id;
            }
        } 
        if(text[i] != '\0')	
			i += 1;
    }
    token_sets_len[mid] = index;
}

// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;


int main(){
	api.init(&n_mails, &n_queries, &mails, &queries);
    
    trie_root = build_node();
    for(int i=0; i<140000; i++) token_check[i] = -1;
    

	mail *t_mail;
	int mid;
	double thres;
    double similarity;
	char *content;
	int ans[10000];
    int ans_len = 0;
    
    
    for(int i=0; i<n_mails; i++){
        token_analysis(i, mails[i].content, 0, trie_root);
        token_analysis(i, mails[i].subject, token_sets_len[i], trie_root);
    }
    /* 
    printf("%d", string_num); 
    for(int i=0; i<n_mails; i++){
        printf("mid:%d\n", i);
        for(int j=0; j<token_sets_len[i]; j++){
            printf("%d ", token_sets[i][j]);
        }
        printf("\n");
    }*/
	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == find_similar){
            //printf("find_similar_query\n");
            mid = queries[i].data.find_similar_data.mid;
            thres = queries[i].data.find_similar_data.threshold;
            ans_len = 0;
            //mark token
            for(int j=0; j<token_sets_len[mid]; j++)
                token_check[token_sets[mid][j]] = mid;
            
            for(int j=0; j<n_mails; j++){
                if(j == mid) continue;
                int intersect_count = 0;

                for(int k=0; k<token_sets_len[j]; k++){
                    if(token_check[token_sets[j][k]] == mid)
                        intersect_count += 1;
                }
                
                similarity = (double) intersect_count / (token_sets_len[mid]+token_sets_len[j]-intersect_count);
                if(similarity > thres){
                    ans[ans_len] = j;
                    ans_len += 1;
                    //printf("conunt:%d\n", intersect_count);
                    //printf("similarity:%f\n", similarity);
                }
                //printf("thres:%f\n", thres);
            }
            //api.answer(i, ans, ans_len);
        }
    }
    
    return 0;
}
