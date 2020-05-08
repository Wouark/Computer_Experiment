/**
 **  scanner.c ... 字句解析（予約語・数・識別子・特殊記号 の判別）
 **/

#include "util.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*===== データ構造の定義 =====*/

struct Token {
  int type;
  int num;    /* type が NUMBER のとき利用 */
  char *id;   /* type が IDENT のとき利用 */
  int row;
  int col;
};

/*===== 非公開の定数 =====*/

enum { MAX_IDENT_LEN = 127 };

static const char *token_type_str[] = {
  "NUMBER",    "IDENT",
  "PLUS",      "MINUS",     "TIMES",     "DIV",       "MOD",
  "GT",        "GEQ",       "LT",        "LEQ",       "EQ",        "NEQ",
  "BECOMES",
  "LPAREN",    "RPAREN",    "LBRACE",    "RBRACE",
  "COMMA",     "SEMICOLON",
  "IF",        "ELSE",      "WHILE",     "RETURN",
  "VAR",       "FUN",
  "VOID",      "INT",
  "END_OF_INPUT",
  "ILLEGAL_TOKEN",   /* エラー処理用 */
  NULL,
};

static const char *reserved_word_str[] = {
  "if",    "else",  "while", "return",
  "var",   "fun",   "void",  "int",
  NULL,
};

static int reserved_word[] = {
  IF,    ELSE,  WHILE, RETURN,
  VAR,   FUN,   VOID,  INT,
  EOA,
};

static char empty_string[1] = { '\0' };

/*===== 非公開の変数 =====*/

static Token tok;
  /* 現在の字句 */
  /* init_lookahead_token() や lookahead_token() を呼び出す更新される */

static int ch;         /* 現在の文字 */
static int row, col;   /* 原始ファイル中での現在の行番号と列番号 */
                       /* lookahead_ch() を呼び出すとこれらが更新される */

static char buf[MAX_IDENT_LEN+1];   /* 識別子の一時記憶 */ /* +1 は \0 の分 */

/*===== 関数定義 =====*/

static int lookahead_ch(void) {
  ch = fgetc(get_file_pointer()); col++;
  if (ch == '\n') {
    row++; col = 1;
  }
  return ch;
}

int   token_type(void) { return tok.type; }
int   token_num(void)  { return tok.num; }
char *token_id(void)   { return tok.id; }
int   token_row(void)  { return tok.row; }
int   token_col(void)  { return tok.col; }

void print_token(void) {
  switch(tok.type) {
  case NUMBER: printf("%s(%d)", token_type_str[tok.type], tok.num); break;
  case IDENT:  printf("%s(%s)", token_type_str[tok.type], tok.id);  break;
  default:     printf("%s",     token_type_str[tok.type]);          break;
  }
}

void init_lookahead_token(void) {
  row = 1; col = 1;
  lookahead_ch();   /* lookahead_token() は常に1字句先読みする */
}

static void get_id_or_reserved_token(void) {
  int i, j;
  const char *s;

  /* 字句の読み込み */
  i = 0;
  do {
    if (i >= MAX_IDENT_LEN) { tok.type = ILLEGAL_TOKEN; return; }
    buf[i] = ch; i++;
    lookahead_ch();
  } while (isalnum(ch) || ch == '_');
  buf[i] = '\0';

  /* 予約語の判定 */
  j = 0;
  s = reserved_word_str[j];
  while (s != NULL && strcmp(buf, s) != 0) {   /* 見つからない */
    j++;
    s = reserved_word_str[j];
  }

  /* 字句の種類の決定 */
  if (s != NULL) {
    tok.type = reserved_word[j];
  } else {
    tok.type = IDENT;
    tok.id = buf;
  }
}

static void get_num_token(void) {
  tok.type = NUMBER;
  tok.num = 0;
  do {
    tok.num = tok.num * 10 + (ch-'0');
  } while (isdigit(lookahead_ch()));
}

int lookahead_token(void) {
  tok.type = ILLEGAL_TOKEN; tok.num = 0; tok.id = empty_string;
  while (isspace(ch)) {
    lookahead_ch();   /* 空白文字を無視 */
  }
  tok.row = row; tok.col = col - 1;   /* -1 は先読み分 */
  if (isalpha(ch) || ch == '_') {
    get_id_or_reserved_token();
  }
  else if (isdigit(ch)) {
    get_num_token();
  }
  else {
    switch(ch) {
    case '+' :    lookahead_ch(); tok.type = PLUS;          break;
    case '-' :    lookahead_ch(); tok.type = MINUS;         break;
    case '*' :    lookahead_ch(); tok.type = TIMES;         break;
    case '/' :    lookahead_ch(); tok.type = DIV;           break;
    case '%' :    lookahead_ch(); tok.type = MOD;           break;
    case '>' :    lookahead_ch();
      switch(ch) {
      case '=':   lookahead_ch(); tok.type = GEQ;           break;
      default :                   tok.type = GT;            break;
      }; break;
    case '<' :    lookahead_ch();
      switch(ch) {
      case '=':   lookahead_ch(); tok.type = LEQ;           break;
      default :                   tok.type = LT;            break;
      }; break;
    case '=' :    lookahead_ch();
      switch(ch) {
      case '=':   lookahead_ch(); tok.type = EQ;            break;
      default :                   tok.type = BECOMES;       break;
      }; break;
    case '!' :    lookahead_ch();
      switch(ch) {
      case '=':   lookahead_ch(); tok.type = NEQ;           break;
      default :                   tok.type = ILLEGAL_TOKEN; break;
      }; break;
    case '(':     lookahead_ch(); tok.type = LPAREN;        break;
    case ')':     lookahead_ch(); tok.type = RPAREN;        break;
    case '{':     lookahead_ch(); tok.type = LBRACE;        break;
    case '}':     lookahead_ch(); tok.type = RBRACE;        break;
    case ',':     lookahead_ch(); tok.type = COMMA;         break;
    case ';':     lookahead_ch(); tok.type = SEMICOLON;     break;
    case EOF :                    tok.type = END_OF_INPUT;  break;
    default :     lookahead_ch(); tok.type = ILLEGAL_TOKEN; break;
    }
  }
  return tok.type;
}
