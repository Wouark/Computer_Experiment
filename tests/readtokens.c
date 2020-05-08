/**
 **  readtokens.c ... 字句列を読み，各字句を位置情報と共に表示
 **
 **  コンパイル： gcc readtokens.c libcprime.a -o readtokens または make
 **
 **  使い方： ./readtokens [TEXT_FILE]
 **           [ ] 内は省略可能
 **/

#include "cprime.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  /* 前処理 */
  open_file_or_exit(argc, argv);
  init_lookahead_token();
  lookahead_token();   /* 1字句先読み */

  /* 字句を読んで行番号と共に表示 */
  while (TRUE) {
    printf("line %d, column %d: ", token_row(), token_col());
    print_token();
    putchar('\n');
    if (token_type() == END_OF_INPUT)
      break;
    lookahead_token();   /* 字句の処理を終えたら1字句先読み */
  }

  /* 後処理 */
  close_file();

  return EXIT_SUCCESS;
}
