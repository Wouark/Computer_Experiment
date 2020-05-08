#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "cprime.h"

static hash_t *table[TABLE_SIZE];

static const char *reserved_word_str[] = {
  "if",    "else",  "while", "return",
  "var",   "fun",   "void",  "int",
  NULL,
};


/*
引数の文字列のハッシュ値を返す
文字列の一文字目のASCII コードをTABLE_SIZEで割った余りを返す
*/
static int hash_function(const char *s) {
    return (int)s[0] % TABLE_SIZE;
}



/*
メモリ確保
hash_tを1サイズ確保して返り値として返す
*/
static hash_t *memory_allocation(void){
    hash_t *t;
    t = (hash_t*)malloc(sizeof(hash_t));
    if(t == NULL){
        /*エラー処理*/
        printf("ERROR : mallocエラー\n");
        exit(EXIT_FAILURE);
    }
    return t;
}


/*
tの要素に変数名(var)と番地(adr)を格納
*/
static void add_element(hash_t *t, const char *var, int adr, int name_type, int range_type){
    t->name = var;
    t->address = adr;
    t->name_type = name_type;
    t->range_type = range_type;
    t->next = NULL;
}


void init_table(void){
    int i;
    for (i = 0; i < TABLE_SIZE; i++){
        table[i] = memory_allocation();
        add_element(table[i], NULL, -1, -1, -1);
    }
}


void add_table(const char *var, int adr, int name_type, int range_type){
    int j = 0;
    const char *s = reserved_word_str[j];
    int key = hash_function(var);
    hash_t *tmp = table[key]->next;
    while (s != NULL) {
        if(strcmp(var, s) == 0){
            /*予約語衝突処理*/
            printf("ERROR :変数名%sが予約語と衝突してます。\n", var);
            exit(EXIT_FAILURE);
        }
        j++;
        s = reserved_word_str[j];
    }
    if(tmp == NULL){
        table[key]->next = memory_allocation();
        add_element(table[key]->next, var, adr, name_type, range_type);
        return;
    }
    while(tmp != NULL){
        if(strcmp(tmp->name, var) == 0){
            /*変数名衝突処理*/
            if (tmp->range_type == range_type){
                /*名前、領域が一致する処理*/
                switch (name_type){
                    case FUNCTION:  
                    printf("ERROR : 関数(%s)は既に定義されています\n", var);
                    exit(EXIT_FAILURE);
                    case ARGUMENT:
                    printf("ERROR : 引数(%s)が二重定義です\n", var);
                    exit(EXIT_FAILURE);                
                    case VARIABLE:
                    printf("ERROR : 変数(%s)が二重定義です\n", var);
                    exit(EXIT_FAILURE);     
                    default:
                    printf("ERROR : %sの記号表登録の際の予期せぬエラーです\n", var);
                    exit(EXIT_FAILURE);   
                    }
            }
            else{
                /*名前が一致するが領域が異なる場合の処理*/
                if(tmp->next != NULL){
                    tmp = tmp->next;
                    continue;
                }
                else{
                    break;
                }
            }
        }
        else{
            /*名前が一致しない場合の処理*/
            if(tmp->next != NULL){
                tmp = tmp->next;
                continue;
            }
            else{
                break;
            }
        }
    }
    tmp->next = memory_allocation();
    add_element(tmp->next, var, adr, name_type, range_type);
}


void print_table(void){
    int i;
    hash_t *tmp;
    for (i = 0; i < TABLE_SIZE; i++){
        tmp = table[i]->next;
        printf("table[%d] ",i);
        while(tmp != NULL){
            printf("-[var: %s, address: %d, ", tmp->name, tmp->address);
            switch (tmp->name_type){
            case FUNCTION: printf("name_type: FUNC ");break;
            case VARIABLE: printf("name_type: VAR, ");break;
            case ARGUMENT: printf("name_type: ARG, ");break;
            default: printf("name_type: ? ");break;
            }
            switch (tmp->range_type){
            case LOCAL: printf("range_type: LOCAL");break;
            case GLOBAL: printf("range_type: GLOBAL");break;
            default: printf("range_type: ? ");break;
            }
            printf("]");
            tmp = tmp->next;
        }
        putchar('\n');
    }
}

int range_return(char *var){
    int key = hash_function(var);
    hash_t *tmp = table[key]->next;
    if(tmp == NULL){
        /*エラー処理*/
        printf("ERROR : 変数%sは定義されていません\n", var);
        exit(EXIT_FAILURE);
    }
    /*varのアドレスを返す
      優先順位は 局所 -> 大域
    */

    while(strcmp(tmp->name, var) != 0 || tmp->range_type != LOCAL){
        tmp = tmp->next;
        if(tmp == NULL){
            break;
        }
    }
    if(tmp != NULL){
        return tmp->range_type;
    }

    tmp = table[key]->next;
    while(strcmp(tmp->name, var) != 0 || tmp->range_type != GLOBAL){
        tmp = tmp->next;
        if(tmp == NULL){
            break;
        }
    }
    if(tmp != NULL){
        return tmp->range_type;
    }
    else {
        printf("ERROR : %sは定義されていません\n", var);
        exit(EXIT_FAILURE);
    }
    return -1;

}

int address_return(char *var){
    int key = hash_function(var);
    hash_t *tmp = table[key]->next;
    if(tmp == NULL){
        /*エラー処理*/
        printf("ERROR : 変数%sは定義されていません\n", var);
        exit(EXIT_FAILURE);
    }
    /*varのアドレスを返す
      優先順位は 局所 -> 大域
    */

    while(strcmp(tmp->name, var) != 0 || tmp->range_type != LOCAL){
        tmp = tmp->next;
        if(tmp == NULL){
            break;
        }
    }
    if(tmp != NULL){
        return tmp->address;
    }

    tmp = table[key]->next;
    while(strcmp(tmp->name, var) != 0 || tmp->range_type != GLOBAL){
        tmp = tmp->next;
        if(tmp == NULL){
            break;
        }
    }
    if(tmp != NULL){
        return tmp->address;
    }
    else {
        printf("ERROR : 変数%sは定義されていません\n", var);
        exit(EXIT_FAILURE);
    }
    return -1;
}

int count_table(void){
    int i;
    int count = 0;
    hash_t *tmp;
    for (i = 0; i < TABLE_SIZE; i++){
        tmp = table[i]->next;
        while(tmp != NULL){
            count++;
            tmp = tmp->next;
        }
    }
    return count;
}

void delete_local(void){
    int i;
    hash_t *tmp;
    hash_t *prev;
    for(i = 0; i < TABLE_SIZE; i++){
        prev = table[i];
        tmp = prev->next;
        while(tmp != NULL){
            if(tmp->range_type == LOCAL){
                prev->next = tmp->next;
                if(tmp->next != NULL){
                    free(tmp);
                    tmp = prev->next;
                }
                else{
                    free(tmp);
                    tmp = NULL;
                }
            }
            else{
                prev = prev->next;
                tmp = tmp->next;
            }
        }
    }
}

void delete_function(void){
    int i;
    hash_t *tmp;
    hash_t *prev;
    for (i = 0; i < TABLE_SIZE; i++){
        prev = table[i];
        tmp = prev->next;
        while(tmp != NULL){
            if(tmp->name_type == FUNCTION){
                prev->next = tmp->next;
                if(tmp->next != NULL){
                    free(tmp);
                    tmp = prev->next;
                }
                else{
                    free(tmp);
                    tmp = NULL;
                }
            }
            else{
                prev = prev->next;
                tmp = tmp->next;
            }
        }
    }
}

/*
メモリ解放
*/
static void free_memory(hash_t *t){
    if(t != NULL){
        free_memory(t->next);
        free(t);
    }
}

void free_table(void){
    int i;
    for (i = 0; i < TABLE_SIZE; i++){
        free_memory(table[i]);
        table[i] = NULL;
    }
}
/*
int main (void){

    init_table();
    add_table("x", 0, VARIABLE, GLOBAL);
    add_table("x", 0, ARGUMENT, GLOBAL);
    add_table("y", 1, VARIABLE, GLOBAL);
    add_table("xx", 2, VARIABLE, LOCAL);
    add_table("sy", 3, VARIABLE, GLOBAL);;
    
    add_table("sub", 4, FUNCTION, GLOBAL);
    //add_table("sub", 4, VARIABLE, "sub");
    add_table("so", 4, FUNCTION, GLOBAL);
    add_table("sx", 4, ARGUMENT, LOCAL);
    add_table("axs", 4, FUNCTION, GLOBAL);
    add_table("z", 5, VARIABLE, LOCAL);
    print_table();
    delete_local();
    putchar('\n');
    printf("局所変数消去\n");
    putchar('\n');
    print_table();
    delete_function();
    putchar('\n');
    printf("関数消去\n");
    putchar('\n');
    print_table();

    free_table();
    return 0;
}
*/

