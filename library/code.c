/**
 **  code.c ... Pコードの 読み込み・生成・格納
 **/

#include "util.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*===== 非公開の定数 =====*/

enum {
  INS_TYPE_LEN = 3,
  MAX_CODE_LINES = 1024,
  PRINT_DIGITS = 4
};

static const char *instruction_type_str[] = {
  "LDC", "LOD", "LDA", "ILD",
  "STR", "ISR",
  "AOP", "COP",
  "WLN", "RLN",
  "STP", "UJP", "FJP",
  "XST", "MST", "CAL", "RET",
  NULL   /* 番兵 */
};

/*===== 非公開の変数 =====*/

static Instruction *seq[MAX_CODE_LINES];   /* 命令列 */
static int last = -1;                      /* 最後の命令の番地 */

/*===== 関数定義 =====*/

static void         add(Instruction *x) { last++; seq[last] = x; }
static Instruction *get(int i)          { return seq[i]; }
static int          is_full(void)       { return (last+1 >= MAX_CODE_LINES); }
static int          is_valid(int addr)  { return (0 <= addr && addr <= last); }

static int str_to_instruction_type(const char* str) {
  int type = 0;
  const char *s = instruction_type_str[type];
  while (s != NULL && strcmp(str, s) != 0) {   /* 見つからない */
    type++;
    s = instruction_type_str[type];
  }
  if (s == NULL) return ERR;
  return type;
}

int get_code_address(void) { return last; }

int append_instruction(int type, int arg1, int arg2) {
  Instruction *insp;
  if (is_full()) return FAILURE;
  insp = (Instruction *) malloc(sizeof *insp);
  if (insp == NULL) return FAILURE;
  insp->type = type; insp->arg1 = arg1; insp->arg2 = arg2;
  add(insp);
  return SUCCESS;
}

int rewrite_instruction(int code_addr, int jump_addr) {
  Instruction *insp;
  if (! is_valid(code_addr)) return FAILURE;
  insp = get(code_addr);
  if (insp == NULL) return FAILURE;
  insp->arg2 = jump_addr;
  return SUCCESS;
}

Instruction *get_instruction(int addr) {
  if (! is_valid(addr)) return NULL;
  return get(addr);
}

void print_address(int addr) {
  printf("%*d:  ", PRINT_DIGITS, addr);
}

void print_instruction(const Instruction *insp) {
  printf("%*s%*d%*d",
         INS_TYPE_LEN, instruction_type_str[insp->type],
         PRINT_DIGITS, insp->arg1,
         PRINT_DIGITS, insp->arg2);
}

void print_code(int visible_addr) {
  int i;
  for (i = 0; i <= last; i++) {
    if (visible_addr)
      print_address(i);
    print_instruction(get_instruction(i));
    putchar('\n');
  }
}

static void exit_code(const char *message) {
  fprintf(stderr, "*** %s ***\n", message);
  fprintf(stderr, "address = %d\n", last+1);
  exit (EXIT_FAILURE);
}

void read_code_or_exit(void) {
  int c;
  do {
    /* 先に1字読んで判別 */
    c = fgetc(get_file_pointer());
    if (isspace(c) || c == EOF) {   /* 空白文字を無視 */
    }
    else if (c == '#') {            /* 注釈行を無視 */
      do {
        c = fgetc(get_file_pointer());
      } while (c != EOF && c != '\n');
    }
    else {   /* 命令の 読み込み・検査・記憶 */
      char type[INS_TYPE_LEN+1];   /* +1 は \0 の分 */
      int ins, arg1 = 0, arg2 = 0, result;
      type[0] = c;
      result = fscanf(get_file_pointer(), "%2s%d%d", &type[1], &arg1, &arg2);
      if (result == EOF || result < 3)
        type[0] = '\0';
      ins = str_to_instruction_type(type);
      if (ins == ERR)
        exit_code("Illegal instruction is detected.");
      if (append_instruction(ins, arg1, arg2) == FAILURE)
        exit_code("No more instruction can be stored.");
    }
  } while (c != EOF);
}
