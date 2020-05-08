/**
 **  util.h ... 汎用関数群
 **/

#ifndef _UTIL_H_INCLUDED_
#define _UTIL_H_INCLUDED_

#include <stdio.h>

/*----- 公開する定数 -----*/

enum Boolean {
  TRUE  = (0 == 0),
  FALSE = (0 == 1)
};

enum ErrorStatus {
  SUCCESS = TRUE,
  FAILURE = FALSE
};

enum Array {
  EOA = -1   /* 配列の末尾 (番兵) */
};

/*----- 公開する関数 -----*/

void open_file_or_exit(int argc, char *argv[]);
/* コマンドでファイルの指定があればそれを開き，なければ標準入力を使う
 * ファイルを開けなければ強制終了
 */
FILE *get_file_pointer(void);
/* 使用中のファイルへのポインタを返す
 */
void close_file(void);
/* 使用中のファイルを閉じる
 */

void set_verbosity(int is_verb);
/* 詳細表示の有無を is_verb の TRUE/FALSE で設定（標準では FALSE）
 */
int is_verbose(void);
/* 詳細表示の有無を TRUE/FALSE で返す
 */

void print_characters(char c, int n);
/* 文字 c を n 回表示
 */

#endif /* _UTIL_H_INCLUDED_ */
