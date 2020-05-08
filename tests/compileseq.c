/**
 **  compileseq.c ... コンマ区切りの数列を読み，それを順に出力するPコードに変換
 **
 **  コンパイル： gcc compileseq.c cprime.a -o compileseq または make
 **
 **  使い方： compileseq [TEXT_FILE]
 **           [ ] 内は省略可能
 **/

#include "cprime.h"
#include <stdio.h>
#include <stdlib.h>

/*========== 補助関数：誤り処理 ==========*/

static void error(const char *str) {
  fputs(str, stderr);
  exit(EXIT_FAILURE);
}

/*========== 主関数：構文解析とコード生成 ==========*/

/*
 * 数の列 seq の構文
 *
 * seq →  NUMBER COMM …  END_OF_INPUT
 */

static void compile_seq(void) {
  while (TRUE) {
    if (token_type() != NUMBER) error("*** NUMBER expected ***\n");
    append_instruction(LDC, 0, token_num());
    append_instruction(WLN, 0, 0);
    lookahead_token();
    if (token_type() != COMMA) break;
    lookahead_token();
  }
  if (token_type() != END_OF_INPUT) error("*** END_OF_INPUT expected ***\n");
  append_instruction(STP, 0, 0);
}

/*========== プログラム本体 ==========*/

int main(int argc, char *argv[]) {

  open_file_or_exit(argc, argv);
  init_lookahead_token();   /* 字句解析器を初期化 */
  lookahead_token();        /* 字句を常に一つ先読み */

  compile_seq();            /* コンパイル処理 */
  print_code(FALSE);        /* 番地を付けずに目的コードを表示 */

  close_file();

  return EXIT_SUCCESS;
}
