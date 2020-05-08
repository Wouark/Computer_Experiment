#define TABLE_SIZE 17 /*ハッシュ表のサイズ*/

enum Name_type {
    VARIABLE,
    ARGUMENT,
    FUNCTION
};

enum Range_type{
    GLOBAL,
    LOCAL,
};

typedef struct hash_t{
    const char *name;
    int address;
    int name_type;
    int range_type;
    struct hash_t *next;
} hash_t;


/*----- 公開する関数 -----*/

void add_table(const char *var, int adr, int name_type, int range_type);
/*
テーブルに変数と番地、名前の種類、範囲を追加 
*/

void init_table(void);
/*
table初期化
*/

void delete_local(void);
/*s
記号表から局所変数として登録されている記号を消去する
関数そのものは残る
*/

void delete_function(void);
/*
ページ表から関数名を消去する
*/

void print_table(void);
/*
記号表(table)の状態を表示
*/

int range_return(char *var);
/*
varがGLOBALかLOCALかを返す。
*/

int address_return(char *var);
/*
varの現在の番地を返す
parentを親にもつ変数がある場合はその番地を返す
ない場合は大域変数上で再検索し、見つかった場合は
その大域変数の番地を返す。
どちらでも見つからなかった場合変数未定義として
エラー処理
*/


void free_table(void);
/*
テーブルメモリ解放
*/

int count_table(void);
/*
記号表に登録されている変数の個数を返す
*/
