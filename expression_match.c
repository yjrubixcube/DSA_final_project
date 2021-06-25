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
    int type, ssize; //0:(, 1:), 2:not, 3:and, 4:or, 5:bool
    bool exp;
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
            if(token_check[token_id] != ((mid == 0) ? -1 : mid)){
                token_check[token_id] = (mid == 0) ? -1 : mid;
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
    new->next = NULL;
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

void spush(stack **stk, int type, bool exp){
    stack *new = (stack*)malloc(sizeof(stack));
    new->type = type;
    new->exp = (type==5) ? exp : false;
    new->next = *stk;
    new->ssize = (*stk==NULL) ? 1 : 1+(*stk)->ssize;
    //return new;
    *stk = new;
}

void spop(stack **stk){
    stack *buf = (*stk)->next, *rtn = *stk;
    free(rtn);
    *stk = buf;
    //return rtn;
}

//this adds all the tokens in the expression into trie
//TODO
//makes a queue of expression
void preprocess(char *exp, trie_node *root, queue *q){
    int index = 0;
    while(exp[index]!='\0'){
        int length=0;
        while ((exp[index+length]<='z' && exp[index+length]>='a')||(exp[index+length]<='Z' && exp[index+length]>='A')|| (exp[index+length] <= '9' && exp[index+length] >= '0')){
            length+=1;
        }
        if (length>0){
            char ins[length], temp = exp[index+length];
            exp[index+length]='\0';
            strcpy(ins, exp+index);
            int token_id = get_token_id(root, ins);
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

void push_opr(stack **s, int opr, bool eval){
    if (eval){
        bool e1 = (*s)->exp, e2;
        spop(s);
        if (opr == 3){//and
            e2 = (*s)->exp;
            spop(s);
            spush(s, 5, e1&&e2);
        }
        else if (opr == 4){//or
            e2 = (*s)->exp;
            spop(s);
            spush(s, 5, e1||e2);
        }
        else if (opr == 2){//not
            spush(s, 5, !e1);
        }
    }
    else{
        spush(s, opr, false);
    }
}

//TODO maybe add parameter of which mail
bool eval(queue *q, int mid){
    qnode *cur = q->head;
    /*
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
            s = spush(s, cur->type, false);
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
            s = spush(s, 5, b);
            break;
        case 1:
            //bool b1, b2;
            b1 = s->exp;
            s = spop(s);
            if (s->type == 2){
                s = spop(s);
                s = spop(s);
                s = spush(s, 5, !b1);
            }
            else if (s->type == 0){
                s = spop(s);
                s = spush(s, 5, b1);
            }

            if (s->ssize>=3){
                if (s->next->type == 2){
                    b1 = s->exp;
                    s = spop(s);
                    s = spop(s);
                    s = spush(s, 5, !b1);
                }
                else if (s->next->type == 3){
                    b1 = s->exp;
                    s = spop(s);
                    s = spop(s);
                    b2 = s->exp;
                    s = spop(s);
                    s = spush(s, 5, b1&&b2);
                }
                else if (s->next->type == 4){
                    b1 = s->exp;
                    s = spop(s);
                    s = spop(s);
                    b2 = s->exp;
                    s = spop(s);
                    s = spush(s, 5, b1||b2);
                }
            }
        }
        cur = cur->next;
    }
    bool b1, b2;
    while (s->ssize>=2){
        if (s->next->type==2){
            b1 = s->exp;
            s = spop(s);
            s = spop(s);
            s = spush(s, 5, !b1);
        }else if (s->next->type == 3){
            b1 = s->exp;
            s = spop(s);
            s = spop(s);
            b2 = s->exp;
            s = spop(s);
            s = spush(s, 5, b1&&b2);
        }else if (s->next->type == 4){
            b1 = s->exp;
            s = spop(s);
            s = spop(s);
            b2 = s->exp;
            s = spop(s);
            s = spush(s, 5, b1||b2);
        }
    }
    return s->exp;

    */
    stack *s=NULL, *buf_s = NULL; 
    while (cur!=NULL){
        if (cur->type<5){//is oprerator
            if (cur->type == 4){//or
                while (buf_s!=NULL && buf_s->type!= 0 && buf_s->type!=2){
                    push_opr(&s, buf_s->type, true);
                    spop(&buf_s);
                }
                push_opr(&buf_s, cur->type, false);
            }
            else if (cur->type == 3){//and
                while (buf_s!=NULL && buf_s->type!=0 && buf_s->type!=2 && buf_s->type!=4){
                    push_opr(&s, buf_s->type, true);
                    spop(&buf_s);
                }
                push_opr(&buf_s, cur->type, false);
                
            }
            else if (cur->type == 2){//not
                while (buf_s!=NULL && buf_s->type!=0){
                    push_opr(&s, buf_s->type, true);
                    spop(&buf_s);
                }
                push_opr(&buf_s, cur->type, false);

            }
            else if (cur->type == 1){//)
                while (buf_s!=NULL && buf_s->type!= 0){
                    push_opr(&s, buf_s->type, true);
                    spop(&buf_s);
                }
                spop(&buf_s);
            }
            else{//(
                push_opr(&buf_s, cur->type, false);
            }
        }
        else{//is id
            bool flag=false;
            for (int i = 0;i<token_sets_len[mid];i++){
                if (cur->id == token_sets[mid][i]){
                    flag = true;
                    break;
                }
            }
            spush(&s, 5, flag);
        }
        cur = cur->next;
    }
    while (buf_s!=NULL){
        push_opr(&s, buf_s->type, 1);
        spop(&buf_s);
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
    //printf("anal\n");
    for(int i=0; i<n_mails; i++){
        token_analysis(i, mails[i].content, 0, trie_root);
        token_analysis(i, mails[i].subject, token_sets_len[i], trie_root);
    }
    //printf("done anal\n");
	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == expression_match){
            char *expression = queries[i].data.expression_match_data.expression;
            queue *q = (queue *)malloc(sizeof(queue));
            q->head = NULL;
            q->tail = NULL;
            q->len = 0;
            //printf("preprocess\n");
            preprocess(expression, trie_root, q);
            int ids[10000]={0}, counter=0;
            //printf("done pp\n");
            for (int j = 0;j<n_mails;j++){
                //printf("%d, ", j);
                if (eval(q, j)){ //TODO
                    ids[counter]=j;
                    counter++;
                }
            }
            //printf("%d, %d\n", i, counter);
            api.answer(i, ids, counter);
        }
    }
    
    return 0;
}
