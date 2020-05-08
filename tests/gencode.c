/**
 **  gencode.c ... Pコードの生成と表示
 **
 **  コンパイル： gcc gencode.c cprime.a -o gencode または make
 **
 **  実行： ./gencode
 **/

#include "cprime.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int visible_addr = TRUE;   /* 番地を表示 */

  /* (7+8)-9 の値を計算して表示する命令列を生成 */
  append_instruction(LDC, 0, 7);
  append_instruction(LDC, 0, 8);
  append_instruction(AOP, 0, 0);
  append_instruction(LDC, 0, 9);
  append_instruction(AOP, 0, 1);
  append_instruction(WLN, 0, 0);
  append_instruction(STP, 0, 0);

  /* 生成した目的コードを出力 */
  print_code(visible_addr);

  return EXIT_SUCCESS;
}
