/**
 **  rewriteaddress.c ... Pコードの飛び先番地を変更
 **
 **  コンパイル： gcc rewriteaddress.c pcode.a -o rewriteaddress または make
 **
 **  実行： ./rewriteaddress
 **/

#include "cprime.h"
#include <stdio.h>
#include <stdlib.h>

int main (void) {
  int visible_addr = TRUE;    /* 目的コードの番地の表示：あり */
  int code_addr, jump_addr;   /* 番地の一時記憶用 */

  append_instruction(RLN, 0, 0);
  append_instruction(FJP, 0, UNKNOWN_ADDRESS);   /* 仮の飛び先を設定 */
  code_addr = get_code_address();                /* 命令の番地を取得 */
  /* code_addr を FJP 命令の番地に設定 */
  append_instruction(LDC, 0, 100);
  append_instruction(WLN, 0, 0);
  jump_addr = get_code_address() + 1;            /* 飛び先番地を取得 */
  /* jump_addr を WLN 命令の次の番地に設定 */
  append_instruction(STP, 0, 0);

  print_code(visible_addr);
  printf("code_addr = %d\n", code_addr);
  printf("jump_addr = %d\n", jump_addr);

  rewrite_instruction(code_addr, jump_addr);     /* 飛び先番地を確定 */
  /* FJP 命令の飛び先を WLN 命令の次の番地に変更 */

  print_code(visible_addr);

  return EXIT_SUCCESS;
}
