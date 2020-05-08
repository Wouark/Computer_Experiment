// compile.c ... C'プログラムを読んでPコードに変換
//
// コンパイル： gcc compile.c libcprime.a -o compile または make
//
// 使い方： ./compile [-v] [C'_PROGRAM_FILE]
//          -v で詳細表示，[ ] 内は省略可能

#include "cprime.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//===== 補助関数：誤り処理，段付け表示 =====

static void exit_parser(const char *message) {
  fprintf(stderr, "*** ERROR : %s ***", message);
  fprintf(stderr, " line=%d, column=%d\n", token_row(), token_col());
  exit(EXIT_FAILURE);   // 強制終了
} 

static void show_message(const char *str) {
  if (is_verbose()) {
    printf("#\t\t\t\t%s\n", str);
  }
} 

//========== 補助関数：字句取得，字句照合，構文解析初期化 ==========

static void read_token(void) {
  lookahead_token();
  if (is_verbose()) {
    printf("# "); print_token(); putchar('\n');
  }
}

static void match_token(int type, const char *str) {
  if (token_type() != type) {
    exit_parser(str);
  }
  read_token();
}

static void init_parse(void) {
  init_lookahead_token();
  read_token();   // 構文解析の各関数は字句を常に一つ先読みする
}

//========== 補助関数：コード生成 ==========

static void emit_instruction(int op, int m, int n, const char *str) { 
  int successful = append_instruction(op, m, n);
  if (is_verbose()) {
    printf("#\t\t%s\n", str);
  }
  if (! successful) {
    exit_parser("code buffer is full");
  }
} 

//========== 補助関数：構文解析 ==========

static void expression();
// expression() は，先に定義される atom() から呼ばれる

//----- 原子式 atom の解析 -----

static void atom() {
  show_message("begin atom");
  int op;
  op = token_type();
  if (op == NUMBER) {
    emit_instruction(LDC, 0, token_num(),"LDC (NUMBER)");
    read_token();
  }
  else if (op == LPAREN) {
    read_token();
    expression();
    match_token(RPAREN, "ERROR : ')' not found");
  }
  else if (op == IDENT){
    int is_input = (strcmp(token_id(), "input") == 0);
    if(is_input){
      read_token();
      match_token(LPAREN, "ERROR : '(' not found");
      match_token(RPAREN, "ERROR : ')' not found");
      emit_instruction(RLN, 0, 0, "RLN (INPUT)");
    }
    else{
      //識別子の処理
      char *id_name = strdup(token_id());
      read_token();
      if(token_type() == LPAREN){
        //**識別子(式)の処理**
        //関数の呼び出し
        //id_nameは関数名　 
        emit_instruction(MST, 0, 0, "MST");
        
        match_token(LPAREN, "ERROR : '(' not found");
        int arg_count = 0;
        if(token_type() != RPAREN){
          arg_count++;
          expression();
          while(token_type() == COMMA){
            read_token();
            expression();
            arg_count++;
          }
        }
        match_token(RPAREN, "ERROR : ')' not found");
        emit_instruction(CAL, arg_count ,address_return(id_name), "CAL");
      }
      else{
        emit_instruction(LOD, range_return(id_name), address_return(id_name), "LOD");
      }
    }
  }
  else {
    exit_parser("ERROR : atomic expression not found");
  }
  show_message("end atom");
}

//----- 因子 factor の解析 -----

static void factor() {
  int op;
  show_message("begin factor");
  op = token_type();
  if (op == END_OF_INPUT){
    exit_parser("ERROR : unexpected END_OF_INPUT");
  }
  

  if (op == PLUS || op == MINUS) {
    emit_instruction(LDC, 0, 0, "LDC (NUMBER)");
    read_token();
  }
  atom();
  if(op == PLUS){
    emit_instruction(AOP, 0, 0, "AOP (PLUS)");
  }
  else if (op == MINUS){
    emit_instruction(AOP, 0, 1, "AOP (MINUS)");
  }
  show_message("end factor");
}

//----- 項 term の解析 -----

static void term() {
  int op;
  show_message("begin term");
  factor();
  op = token_type();
  while (op == TIMES || op == DIV || op == MOD) {
    read_token();
    factor();
    switch (op){
    case TIMES: emit_instruction(AOP, 0, 2, "AOP (TIMES)"); break;
    case DIV: emit_instruction(AOP, 0, 3, "AOP (DIV)"); break;
    case MOD: emit_instruction(AOP, 0, 4, "AOP (MOD)"); break;
    default: exit_parser("ERROR : NOT TIMES or DIV or MOD"); break;
    }
    op = token_type();
  }
  show_message("end term");
}

//----- 算術式 arithmetic の解析 -----

static void arithmetic() {
  int op;
  show_message("begin arithmetic");
  term();
  op = token_type();
  while (op == PLUS || op == MINUS) {
    read_token();
    term();
    switch (op){
    case PLUS: emit_instruction(AOP, 0, 0, "AOP (PLUS)"); break;
    case MINUS: emit_instruction(AOP, 0, 1, "AOP (MINUS)"); break;
    default: exit_parser("ERROR : NOT PLUS or MINUS"); break;
    }
    op = token_type();
  }
  show_message("end arithmetic");
}

//----- 比較 comparison の解析 -----

static void comparison() {
  show_message("begin comparison");
  arithmetic();
  int op = token_type();
  while(op == GT || op == GEQ || op == LT || op == LEQ){
    read_token();
    arithmetic();
   // > , >= , < , <= の処理
    switch (op){
    case GT:    emit_instruction(COP, 0, 0, "COP (GT)"); break;
    case GEQ:   emit_instruction(COP, 0, 1, "COP (GEQ)"); break;
    case LT:    emit_instruction(COP, 0, 2, "COP (LT)"); break;
    case LEQ:   emit_instruction(COP, 0, 3, "COP (LEQ)"); break;
    default: exit_parser("ERROR : '>' , '>=' , '<' or '<=' not found"); break;
    }
    op = token_type();
  }
  show_message("end comparison");
}

//----- 式 expression の解析 -----

static void expression() {
  show_message("begin expression");
  comparison();
  int op = token_type();
  while(op == EQ || op == NEQ){
    read_token();
    comparison();
    // == と　!= の処理
    switch (op){
    case EQ:     emit_instruction(COP, 0, 4, "COP (EQ)"); break;
    case NEQ:    emit_instruction(COP, 0, 5, "COP (NEQ)"); break;
    default:    exit_parser("ERROR : '==' or '!=' not found"); break;
    }
    op = token_type();
  }
  show_message("end expression");
}

//----- 文 statement の解析 -----

static void statement() {
  show_message("begin statement");
  int op;
  op = token_type();
  if (op == IDENT) {
    //識別子の場合
    int is_output = (strcmp(token_id(), "output") == 0);
    if(is_output){
      //out_putの処理
      read_token();
      match_token(LPAREN, "ERROR : '(' not found");
      if(token_type() != RPAREN){
        expression();
      }
      match_token(RPAREN, "ERROR : ')' not found");
      emit_instruction(WLN, 0, 0, "WLN");
    }
    else {
      //output でない場合
      char *id_name = strdup(token_id());
      read_token();
      if(token_type() == LPAREN){
        //**識別子(式)の処理**
        //関数の呼び出し
        //id_nameは関数名　 
        emit_instruction(MST, 0, 0, "MST");
        
        match_token(LPAREN, "ERROR : '(' not found");
        int arg_count = 0;
        if(token_type() != RPAREN){
          arg_count++;
          expression();
          while(token_type() == COMMA){
            read_token();
            expression();
            arg_count++;
          }
        }
        match_token(RPAREN, "ERROR : ')' not found");
        emit_instruction(CAL, arg_count ,address_return(id_name), "CAL");
      }
      else{
        //**識別子 = 式の処理**//
        //id_nameは変数名
        match_token(BECOMES, "ERROR : '=' not found");
        expression();
        emit_instruction(STR, range_return(id_name), address_return(id_name), "STR (global)");
      }
    }
    match_token(SEMICOLON, "ERROR : ';' not found");
  }
  else if (op == IF){
    //if文処理
    read_token();
    match_token(LPAREN, "'(' not found");
    expression();
    match_token(RPAREN, "')' not found");
    emit_instruction(FJP, 0, UNKNOWN_ADDRESS, "FJP (if)");
    int if_adr = get_code_address();
    statement();
    if(token_type() == ELSE){
      emit_instruction(UJP, 0, UNKNOWN_ADDRESS, "UJP (else)");
      int else_adr = get_code_address();
      rewrite_instruction(if_adr, else_adr + 1);
      read_token();
      statement();
      rewrite_instruction(else_adr, get_code_address() + 1);
    }
    else{
      rewrite_instruction(if_adr, get_code_address() + 1);
    }
  }
  else if (op == WHILE){
    //while文処理
    read_token();
    int exp_adr = get_code_address() + 1;
    match_token(LPAREN, "'(' not found");
    expression();
    match_token(RPAREN, "')' not found");
    emit_instruction(FJP, 0, UNKNOWN_ADDRESS, "FJP (While)");
    int fjp_adr = get_code_address();
    statement();
    emit_instruction(UJP, 0, exp_adr, "UJP (while)");
    rewrite_instruction(fjp_adr, get_code_address() + 1);
  }
  else if (op == LBRACE){
    // {式}の処理
    read_token();
    while(token_type() != RBRACE){
      statement();
    } 
    match_token(RBRACE,"'}' not found" );
  }
  else if (op == RETURN){
    //return処理
    read_token();
    if(token_type() != SEMICOLON){
      //返り値を持つ場合
      expression();
      match_token(SEMICOLON, "ERROR : ';', not found");
      emit_instruction(STR, LOCAL, 0, "STR");
    }
    else{
      //返り値をもたない場合
      read_token();
    }
  }
  
  else {
    printf("%d\n", token_type());
    exit_parser("identifier, while, if, or '{' not found");
  }
  show_message("end statement");
}

//----- 変数宣言 declaration の解析 -----
//
static void declaration(int init_address, int range_type) {
  show_message("begin declaration");
  add_table(strdup(token_id()), init_address, VARIABLE, range_type);
  init_address++;
  match_token(IDENT, "identifier not found");
  while(token_type() == COMMA){
    read_token();
    add_table(strdup(token_id()), init_address, VARIABLE, range_type);
    init_address++;
    match_token(IDENT, "identifier not found");
    }
  if(range_type == LOCAL)
    init_address += -3;
  emit_instruction(XST, 0, init_address, "XST");
  show_message("end declaration");
}
//
//----- 関数定義 definition の解析 -----

static void definition(void) {
  show_message("begin definition");
  int ret_count =  -1;
  if (token_type() == VOID) {
    read_token();
    ret_count = 0;
  }
  else if(token_type() == INT){
    read_token();
    ret_count = 1;
  }
  else {
    exit_parser("ERROR : 'void' not found\n");
  }
  //関数名処理
  int func_adr = get_code_address()+1;
  char *func_name = strdup(token_id());
  int arg_count = 1 + 2;//制御番地文を余分にとる;
  add_table(func_name, func_adr, FUNCTION, GLOBAL);
  match_token(IDENT, "ERROR : identifier not found");
  match_token(LPAREN, "ERROR : '(' not found");
  if(token_type() != RPAREN){
    //引数処理
    if(token_type() == IDENT){
      add_table(strdup(token_id()), arg_count, ARGUMENT, LOCAL);
      arg_count++;
      read_token();
      while(token_type() == COMMA){
        read_token();
        add_table(strdup(token_id()), arg_count, ARGUMENT, LOCAL);
        arg_count++;
        match_token(IDENT, "identifier not found");
      }
    }
    else{
      exit_parser("ERROR 引数が見つかりません\n");
    }
    
  }
  match_token(RPAREN, "ERROR : ')' not found");
  match_token(LBRACE, "ERROR : '{' not found");
  if(token_type() == VAR){
    read_token();
    declaration(arg_count, LOCAL);
    match_token(SEMICOLON, "ERROR : ';' not found");
  }
  else{
    emit_instruction(XST, 0, 0, "MST");
  }
  while (token_type() != RBRACE) {
    statement();
  }
  emit_instruction(RET, 0, ret_count, "RET");
  read_token();
  delete_local();
  show_message("end definition");
}

//----- プログラム program の解析 -----

static void program(void) {
  show_message("begin program");
  int exist_var = 0;
  if(token_type() == VAR){
    exist_var++;
    read_token();
    declaration(0, GLOBAL);
    match_token(SEMICOLON, "ERROR : ';' not found");
  }
  emit_instruction(UJP, 0, UNKNOWN_ADDRESS, "UJP");
  while(token_type() == FUN){
    read_token();
    definition();
  }
  emit_instruction(MST, 0, 0, "MST");
  rewrite_instruction(exist_var, get_code_address());
  emit_instruction(CAL, 0, address_return("main"), "CAL");
  /*else{
    exit_parser("'fun' found");
  }*/
  if (token_type() != END_OF_INPUT) {
    exit_parser("ERROR : end of input not found");
  }
  else {
    delete_function();
    emit_instruction(XST, 0, -count_table(), "XST");
    emit_instruction(STP, 0, 0, "STP");
  }
  show_message("end program");
}

//========== 主関数： C'プログラム全体の構文解析 ==========

int main (int argc, char *argv[]) {
  int is_verb = FALSE;   // 詳細表示：通常はしない
  int visible_addr;      // 目的コードの番地の表示の有無
  // オプション処理
  if (argc > 1 && strcmp(argv[1], "-v") == 0) {       // -v あり
    argc--; argv[1] = argv[0]; argv++;   // argv[1] の -v を削除
    is_verb = TRUE;
  }

  // 前処理
  open_file_or_exit(argc, argv);
  set_verbosity(is_verb);
  init_parse();
  init_table();

  // コンパイル処理
  program();
  visible_addr = FALSE;
  //*** 目的コードを出力 ***
  print_code(visible_addr);

  //*** 記号表を出力 ***
  if(is_verb)
    print_table();
  // 後処理
  free_table();
  close_file();

  return EXIT_SUCCESS;
} 
