/**
 **  util.c ... 汎用関数群
 **/

#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * ファイルアクセス
 */

/*===== 非公開の変数 =====*/

static FILE *filep = NULL;   /* 指定されたファイルへのポインタか stdin */

/*===== 関数定義 =====*/

FILE *get_file_pointer(void) {
  return filep;
}

void open_file_or_exit(int argc, char *argv[]) {
  if (argc == 1) {
    filep = stdin;
  }
  else {
    const char *namep = argv[1];
    filep = fopen(namep, "r");
    if (filep == NULL) {
      fprintf(stderr, "*** File open failed. ***\n"); perror(namep);
      exit(EXIT_FAILURE);
    }
  }
}

void close_file(void) {
  fclose(filep);
  filep = NULL;
}

/*
 * 表示の多さの制御
 */

/*===== 非公開の変数 =====*/

static int verbose = FALSE;

/*===== 関数定義 =====*/

int is_verbose(void) {
  return verbose;
}

void set_verbosity(int is_verb) {
  verbose = is_verb;
}

/*
 * 表示
 */

/*===== 関数定義 =====*/

void print_characters(char c, int n) {
  int i;
  for(i = 0; i < n; i++)
    putchar(c);
}
