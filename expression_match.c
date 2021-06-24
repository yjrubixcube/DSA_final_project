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
trie_node *trie_root; //trie for group_analyse

typedef struct stack
{
    struct stack *next;
    int type; //0:(, 1:), 2:not, 3:and, 4:or, 5:bool
    bool exp;
    char *token;
}stack;

typedef struct qnode
{
    struct qnode *next;
    int id, type; //0:(, 1:), 2:not, 3:and, 4:or, 5:token_id
}qnode;

typedef struct queue
{
    qnode *head, *tail;
    int len;
}queue;

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

void qpush(queue *q, int type, int id){
    qnode *new = (qnode *)malloc(sizeof(qnode));
    new->id = id;
    new->type = type;
    if (q->len==0){
        q->head = new;
        q->tail = new;
    }
    else{
        q->tail->next = new;
        q->tail = new;
    }
    q->len++;
}

stack* spush(stack *stk, int type, bool exp, char *w){
    stack *new = (stack*)malloc(sizeof(stack));
    new->type = type;
    new->exp = (type==5) ? exp : false;
    new->token = w;
    new->next = stk;
    return new;
}

stack *spop(stack *stk){
    stack *rtn = stk->next;
    free(stk);
    return rtn;
}

//create new node
trie_node* build_node(){
    trie_node *new = (trie_node*)malloc(sizeof(trie_node));
    
    for(int i=0; i<36; i++) new->child[i] = NULL;
    new->is_token = false;
    new->id = -1;

    return new;
} 


//this will insert a token into the trie
//prob replaces by token anal, can del
void insert_token(trie_node *root, char *w){
    trie_node *cur = root;
    int level = 0;
    int index;

    while(w[level] != '\0'){
        if (w[level]>='a'){
            index = w[level]-'a';
        }
        else if (w[level]>='A'){
            index = w[level]-'A';
        }
        else if (w[level]>='0'){
            index = w[level]-'0'+26;
        }
        if(cur->child[index] == NULL){
            cur->child[index] = build_node();
        }
        cur = cur->child[index];
        level += 1;
    }
    if (!cur->is_token){
        cur->is_token = true;
    }
}

//check if a token is in the trie
//can delete
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
            index = w[level]-'0'+26;
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
    if (!cur->is_token){
        flag = false;
    }

    return flag;
}

//this adds all the tokens in the expression into trie
//TODO
//makes a queue of expression
void preprocess(char *exp, trie_node *root, stack *stk, queue *q){
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
            int token_id = get_token_id(root, ins);
            //insert_token(root, ins);
            qpush(q, 5, token_id);
            exp[index+length]=temp;
            index+=length;
        }else{
            switch (exp[index])
            {
            case '(':
                qpush(q, 0, -1);
                break;
            
            case ')':
                qpush(q, 1, -1);
                break;
            
            case '!':
                qpush(q, 2, -1);
                break;

            case '&':
                qpush(q, 3, -1);
                break;

            case '|':
                qpush(q, 4, -1);
                break;
            }
            index+=1;
        }
    }
}

//TODO maybe add parameter of which mail
bool eval(queue *q, int mid){
    qnode *cur = q->head;
    stack *s = NULL;
    while (cur!=NULL)
    {
        bool b=false, b1, b2;
        switch (cur->type)
        {
        case 0:
        case 2:
        case 3:
        case 4:
            spush(s, cur->type, false, '\0');
            break;
        case 5:
            //check if the token_id is in trie
            //TODO
            //bool b = false;
            for (int i = 0;i<token_sets_len[mid];i++){
                if (cur->id==token_sets[mid][i]){
                    b = true;
                    break;
                }
            }
            spush(s, 5, b, '\0');
            break;
        case 1:
            //bool b1, b2;
            switch (s->next->type)
            {
            case 0:
                b1 = s->exp;
                s=spop(s);
                s=spop(s);
                s = spush(s, 5, b1, '\0');
                break;
            case 2:
                b1 = s->exp;
                s = spop(s);
                s = spop(s);
                s = spop(s);
                s = spush(s, 5, !b1, '\0');
                break;
            case 3:
                b1 = s->exp;
                b2 = s->next->next->exp;
                s = spop(s);
                s = spop(s);
                s = spop(s);
                s = spop(s);
                s = spush(s, 5, b1&&b2, '\0');
                break;
            case 4:
                b1 = s->exp;
                b2 = s->next->next->exp;
                s = spop(s);
                s = spop(s);
                s = spop(s);
                s = spop(s);
                s = spush(s, 5, b1||b2, '\0');
                break;
            }
        }
        cur = cur->next;
    }
    return s->exp;
    

}
// The testdata only contains the first 100 mails (mail1 ~ mail100)
// and 2000 queries for you to debug.

int n_mails, n_queries;
mail *mails;
query *queries;


int main(){
	api.init(&n_mails, &n_queries, &mails, &queries);
    
    trie_root = build_node(); //initialize root
    for(int i=0; i<140000; i++) token_check[i] = -1;
    
	mail *t_mail;
	int n;

    for(int i=0; i<n_mails; i++){
        token_analysis(i, mails[i].content, 0, trie_root);
        //token_analysis(i, mails[i].subject, token_sets_len[i], trie_root);
    }

	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == expression_match){
            //make a trie for each mail
            //eval the expression
            //for all token
            char *expression = queries[i].data.expression_match_data.expression;
            queue *q = (queue *)malloc(sizeof(queue));
            q->head = NULL;
            q->tail = NULL;
            q->len = 0;

            int ids[10000]={0}, counter=0;
            for (int j = 0;j<n_mails;j++){
                if (eval(q, j)){ //TODO
                    ids[counter]=j;
                    counter++;
                }
            }
            api.answer(i, ids, counter);
        }
    }
    
    return 0;
}
