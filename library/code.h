/**
 **  code.h ... P コードの 読み込み・生成・格納
 **/

#ifndef _CODE_H_INCLUDED_
#define _CODE_H_INCLUDED_

/*----- 公開するデータ型 -----*/

struct Instruction {
  int type;
  int arg1;
  int arg2;
};

typedef struct Instruction Instruction;

/*----- 公開する定数 -----*/

enum InstructionType {
  ERR = -1,   /* エラー処理用 */
  LDC, LOD, LDA, ILD,
  STR, ISR,
  AOP, COP,
  WLN, RLN,
  STP, UJP, FJP,
  XST, MST, CAL, RET
};

enum {
  UNKNOWN_ADDRESS = -1
};

/*----- 公開する関数 -----*/

int get_code_address(void);
/* 格納中の最後の命令の番地を返す
 */
int append_instruction(int type, int arg1, int arg2);
/* 命令 (type, arg1, arg2) を最後に追加し
 * エラーの有無を SUCCESS/FAILURE で返す
 */
int rewrite_instruction(int code_addr, int jump_addr);
/* code_addr 番地に格納中の命令 (type, arg1, arg2) の arg2 を jump_addr に変え
 * エラーの有無を SUCCESS/FAILURE で返す
 */
Instruction *get_instruction(int addr);
/* addr 番地に格納中の命令へのポインタを返す（無効な番地なら NULL を返す）
 */

void print_address(int addr);
/* 番地 addr を表示
 */
void print_instruction(const Instruction *insp);
/* 命令 *insp を表示
 */
void print_code(int visible_addr);
/* 格納中の命令列を表示
 * visible_addr が TRUE なら番地も表示
 */

void read_code_or_exit(void);
/* 使用中のファイルを読んで命令列を格納
 * エラーがあれば強制終了
 */

#endif /* _CODE_H_INCLUDED_ */
