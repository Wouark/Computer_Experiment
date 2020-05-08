/**
 **  scanner.h ... 字句解析（予約語・数・識別子・特殊記号 の判別）
 **/

#ifndef _SCANNER_H_INCLUDED_
#define _SCANNER_H_INCLUDED_

/*----- 公開するデータ型 -----*/

typedef struct Token Token;

/*----- 公開する定数 -----*/

enum TokenType {
  NUMBER,    IDENT,
  PLUS,      MINUS,     TIMES,     DIV,       MOD,
  GT,        GEQ,       LT,        LEQ,       EQ,        NEQ,
  BECOMES,
  LPAREN,    RPAREN,    LBRACE,    RBRACE,
  COMMA,     SEMICOLON,
  IF,        ELSE,      WHILE,     RETURN,
  VAR,       FUN,
  VOID,      INT,
  END_OF_INPUT,
  ILLEGAL_TOKEN   /* エラー処理用 */
};

/*----- 公開する関数 -----*/

int token_type(void);
/* 現在の字句の種類を返す
 */
int token_num(void);
/* 現在の字句が表す数値を返す
 */
char *token_id(void);
/* 現在の字句が表す識別子を返す
 * ポインタが指す識別子は更新され得ることに注意
 */
int token_row(void);
/* 現在の字句が原始ファイルの先頭から何行目かを返す
 */
int token_col(void);
/* 現在の字句が原始ファイルの行頭から何文字かを返す
 */

void print_token(void);
/* 現在の字句を表示
 */

void init_lookahead_token(void);
/* 字句解析の初期化（lookahead_token() を最初に呼ぶ前に実行）
 */
int lookahead_token(void);
/* 次の字句を読み，現在の字句を更新し，字句の種類を返す
 */

#endif /* _SCANNER_H_INCLUDED_ */
