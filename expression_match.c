#include "api.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int string_num = 0;  //number of names stored in the trie 
int q_num=0, s_num=0, bs_num=0;
int token_sets[10000][5000];
int token_sets_len[10000];
int token_check[140000];
int queue_arr[3000][2]; //0 type 1 id
int stack_arr[3000][2];
int buffer_stack_arr[3000][2];
//int **sa = stack_arr, **bsa = buffer_stack_arr;

typedef struct trie_node{
    struct trie_node *child[36];
    int id;  //index in disjoint set array
    bool is_token;
} trie_node;
trie_node *trie_root; //trie for group_analyse

/*typedef struct stack
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
*/

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

void qpush(int type, int id){
    queue_arr[q_num][0] = type;
    queue_arr[q_num][1] = id;
    q_num++;
}

void spush(int arr[3000][2], int type, bool exp, bool which){//0 for s, 1 for buffer
    //(which)? bs_num++ : s_num++;
    if (which){
        arr[bs_num][0]=type;
        arr[bs_num][1]=exp;
        bs_num++;
    }
    else{
        arr[s_num][0] = type;
        arr[s_num][1] = exp;
        s_num++;
    }
}

void spop(bool which){
    //(which)? bs_num++ : s_num++;
    if (which){
        bs_num--;
    }
    else{
        s_num--;
    }
}

//this adds all the tokens in the expression into trie
//TODO
//makes a queue of expression
void preprocess(char *exp, trie_node *root){
    int index = 0;
    q_num = 0;
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
            qpush(5, token_id);
            exp[index+length]=temp;
            index+=length;
        }else{
            switch (exp[index])
            {
            case '(':
                qpush(0, -1);
                break;
            
            case ')':
                qpush(1, -1);
                break;
            
            case '!':
                qpush(2, -1);
                break;

            case '&':
                qpush(3, -1);
                break;

            case '|':
                qpush(4, -1);
                break;
            }
            index+=1;
        }
    }
}

void push_opr(int opr, bool eval, bool which){//0 for s, 1 for buffer
    if (which){
        /*if (eval){
            bool e1 = buffer_stack_arr[bs_num][1], e2;
            spop(which);
            if (opr == 3){//and
                e2 = (bool)buffer_stack_arr[bs_num][1];
                spop(which);
                spush(s, 5, e1&&e2, which);
            }
            else if (opr == 4){//or
                e2 = (bool)buffer_stack_arr[bs_num][1];
                spop(which);
                spush(s, 5, e1||e2, which);
            }
            else if (opr == 2){//not
                spush(s, 5, !e1, which);
            }
        }
        else{*/
        spush(buffer_stack_arr, opr, false, which);
        //}
    }
    else{
        //if (eval){
            bool e1 = stack_arr[s_num][1], e2;
            spop(which);
            if (opr == 3){//and
                e2 = stack_arr[s_num][1];
                spop(which);
                spush(stack_arr, 5, e1&&e2, which);
            }
            else if (opr == 4){//or
                e2 = stack_arr[s_num][1];
                spop(which);
                spush(stack_arr, 5, e1||e2, which);
            }
            else if (opr == 2){//not
                spush(stack_arr, 5, !e1, which);
            }
        /*}
        else{
            spush(s, opr, false, which);
        }*/
    }
}

//TODO maybe add parameter of which mail
bool eval(int mid){
    int cur = 0;
    bs_num = 0;
    s_num = 0;
    while (cur<q_num){
        if (queue_arr[cur][0]<5){//is oprerator
            if (queue_arr[cur][0] == 4){//or
                while (bs_num>0 && buffer_stack_arr[bs_num][0]!= 0 && buffer_stack_arr[bs_num][0]!=2){
                    push_opr(buffer_stack_arr[bs_num][0], true, 0);
                    spop(1);
                }
                push_opr(queue_arr[cur][0], false, 1);
            }
            else if (queue_arr[cur][0] == 3){//and
                while (bs_num>0 && buffer_stack_arr[bs_num][0]!=0 && buffer_stack_arr[bs_num][0]!=2 && buffer_stack_arr[bs_num][0]!=4){
                    push_opr(buffer_stack_arr[bs_num][0], true, 0);
                    spop(1);
                }
                push_opr(queue_arr[cur][0], false, 1);
                
            }
            else if (queue_arr[cur][0] == 2){//not
                while (bs_num>0 && buffer_stack_arr[bs_num][0]!=0){
                    push_opr(buffer_stack_arr[bs_num][0], true, 0);
                    spop(1);
                }
                push_opr(queue_arr[cur][0], false, 1);

            }
            else if (queue_arr[cur][0] == 1){//)
                while (bs_num>0 && buffer_stack_arr[bs_num][0]!= 0){
                    push_opr(buffer_stack_arr[bs_num][0], true, 0);
                    spop(1);
                }
                spop(1);
            }
            else{//(
                push_opr(queue_arr[cur][0], false, 1);
            }
        }
        else{//is id
            bool flag=false;
            for (int i = 0;i<token_sets_len[mid];i++){
                if (queue_arr[cur][1] == token_sets[mid][i]){
                    flag = true;
                    break;
                }
            }
            spush(stack_arr, 5, flag, 0);
        }
        cur++;
    }
    while (bs_num>0){
        push_opr(buffer_stack_arr[bs_num][0], true, 0);
        spop(1);
    }
    
    return stack_arr[0][1];
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
    printf("anal\n");
    for(int i=0; i<n_mails; i++){
        token_analysis(i, mails[i].content, 0, trie_root);
        token_analysis(i, mails[i].subject, token_sets_len[i], trie_root);
    }
    printf("done anal\n");
	for(int i = 0; i < n_queries; i++){
        if(queries[i].type == expression_match){
            char *expression = queries[i].data.expression_match_data.expression;
            //q_num = 0;
            printf("preprocess\n");
            preprocess(expression, trie_root);
            int ids[10000]={0}, counter=0;
            printf("done pp\n");
            for (int j = 0;j<n_mails;j++){
                //printf("%d, ", j);
                if (eval(j)){ //TODO
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
