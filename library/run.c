/**
 **  run.c ... Pコードを読んで仮想スタック計算機上で実行
 **
 **  使い方： ./run [-v] [P_CODE_FILE]
 **           -v で詳細表示，[ ] 内は省略可能
 **/

#include "util.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*===== 非公開の定数 =====*/

enum {
  STACK_SIZE = 1024,
  SIDE_MARGIN = 7, INS_WIDTH = 11,
  PRINT_DIGITS = 4, PRINT_STACK_NUM = 7
};

/*===== 非公開の変数 =====*/

static int stack[STACK_SIZE];   /* 実行スタック */

static Instruction *insp;       /* 命令レジスタ */
static int pc;                  /* プログラムカウンタ */
static int base;                /* ベースポインタ */
static int top;                 /* スタックトップポインタ */

static int r1;                  /* 演算用レジスタ1 */
static int r2;                  /* 演算用レジスタ2 */

/*===== 補助関数：エラー処理 =====*/

static int exit_interpreter(const char *message) {
  fprintf(stderr, "*** %s ***\n", message);
  fprintf(stderr, "pc=%d, base=%d, top=%d\n", pc, base, top);
  exit(EXIT_FAILURE);
}

static void exit_stack(void) {
  exit_interpreter("Stack operation failed.");
}

/*===== 補助関数：スタック操作 =====*/

static void push(int x) {
  if (top >= STACK_SIZE-1) exit_stack();
  top++; stack[top] = x;
}

static int pop(void) {
  int x;
  if (top < 0) exit_stack();
  x = stack[top]; top--; return x;
}

static int get(int i) {
  if (i < 0 || top < i) exit_stack();
  return stack[i];
}

static void put(int i, int x) {
  if (i < 0 || top < i) exit_stack();
  stack[i] = x;
}

static void expand(int i) {
  top += i;
  if (top < -1 || STACK_SIZE <= top) exit_stack();
}

static void set_top(int i) {
  top = i;
  if (top < -1 || STACK_SIZE <= top) exit_stack();
}

static void set_base(int i) {
  base = i;
  if (base < 0 || STACK_SIZE <= base) exit_stack();
}

static int get_base(int is_local) {
  return (is_local ? base : 0);
}

/*===== 補助関数：表示 =====*/

static void print_initial_message(void) {
    int visible_addr = TRUE;
    puts("Instructions:");
    putchar('\n');
    print_code(visible_addr);
    putchar('\n');
    puts("Program started.");
    putchar('\n');
    print_characters(' ', SIDE_MARGIN);
    puts("instruction |   pc | base |  top | stack");
    print_characters(' ', SIDE_MARGIN);
    puts("------------+------+------+------+-----------------------------------");
    print_characters(' ', SIDE_MARGIN + INS_WIDTH);
}

static void print_current_instruction(void) {
  print_characters(' ', SIDE_MARGIN);
  print_instruction(get_instruction(pc));
}

static void print_current_status(void) {
  int i, start;
  /* 主要レジスタ表示 */
  printf(" | %*d | %*d | %*d |",
         PRINT_DIGITS, pc, PRINT_DIGITS, base, PRINT_DIGITS, top);
  /* スタックの表示開始位置設定 */
  start = top - PRINT_STACK_NUM + 1;
  if (start < 0) start = 0;
  /* 省略の有無の表示 */
  putchar((start > 0) ? ':' : ' ');
  /* スタックの内容の表示 */
  for (i = start; i < start + PRINT_STACK_NUM; i++) {
    if (top >= 0 && i <= top) printf( "%*d ", PRINT_DIGITS, get(i));
    else                      printf("%*s* ", PRINT_DIGITS-1, "");
  }
  putchar('\n');
}

static void print_final_message(void) {
  print_current_status();
  putchar('\n');
  printf("Program halted.\n\n");
}

/*===== 補助関数：命令の解釈 =====*/

static void init_interpret(void) {
  insp = NULL; pc = 0; base = 0; top = -1;
  r1 = 0; r2 = 0;
}

static void execute_current_instruction(void) {
  int a1 = insp->arg1, a2 = insp->arg2;   /* 略記の導入 */
  switch (insp->type) {
  case LDC:                                     push(a2);    break;
  case LOD: r1 = get_base(a1); r2 = get(r1+a2); push(r2);    break;
  case LDA: r1 = get_base(a1);                  push(r1+a2); break;
  case ILD: r1 = pop();        r2 = get(r1);    push(r2);    break;
  case STR: r1 = pop(); r2 = get_base(a1)+a2; put(r2, r1); break;
  case ISR: r1 = pop(); r2 = pop();           put(r2, r1); break;
  case AOP: r1 = pop(); r2 = pop();
            switch (a2) {
            case 0: push(r2 + r1); break;
            case 1: push(r2 - r1); break;
            case 2: push(r2 * r1); break;
            case 3: push(r2 / r1); break;
            case 4: push(r2 % r1); break;
            default: break;
            }
            break;
  case COP: r1 = pop(); r2 = pop();
            switch (a2) {
            case 0: push(r2 >  r1); break;
            case 1: push(r2 >= r1); break;
            case 2: push(r2 <  r1); break;
            case 3: push(r2 <= r1); break;
            case 4: push(r2 == r1); break;
            case 5: push(r2 != r1); break;
            default: break;
            }
            break;
  case WLN: r1 = pop();
            if (is_verbose()) putchar('\n');
            printf("%d\n", r1);
            if (is_verbose()) print_characters(' ', SIDE_MARGIN + INS_WIDTH);
            break;
  case RLN: if (is_verbose()) putchar('\n');
            r1 = 0;
            scanf("%d", &r1);
            push(r1);
            if (is_verbose()) print_characters(' ', SIDE_MARGIN + INS_WIDTH);
            break;
  case STP:                                   break;
  case UJP:                          pc = a2; break;
  case FJP: r1 = pop(); if (r1 == 0) pc = a2; break;
  case XST:                      expand(a2);   break;
  case MST: push(0); push(base); expand(1+a1); break;
  case CAL: put(top-a1, pc); set_base(top-(3+a1)+1); pc = a2; break;
  case RET: set_top(base+3-1);
            pc = pop(); r1 = pop(); set_base(r1); if (a2 == 0) pop();
            break;
  default:  exit_interpreter("Illegal instruction is detected.");
  }
}

void interpret_or_exit(void) {
  init_interpret();
  read_code_or_exit();
  if (is_verbose())
    print_initial_message();
  do {
    if (is_verbose())
      print_current_status();
    insp = get_instruction(pc);
    if (insp == NULL) exit_interpreter("Loading instruction failed.");
    if (is_verbose())
      print_current_instruction();
    pc++;
    execute_current_instruction();
  } while (insp->type != STP);
  if (is_verbose())
    print_final_message();
}

/*===== 主関数：Pコードの読み込みと実行 =====*/

int main(int argc, char *argv[]) {
  int is_verb = FALSE;                     /* 詳細表示：通常なし */
  if (argc > 1 && strcmp(argv[1], "-v") == 0) {       /* -v あり */
    argc--; argv[1] = argv[0]; argv++;   /* argv[1] の -v を削除 */
    is_verb = TRUE;
  }
  open_file_or_exit(argc, argv);
  set_verbosity(is_verb);
  interpret_or_exit();
  close_file();
  return EXIT_SUCCESS;
}
