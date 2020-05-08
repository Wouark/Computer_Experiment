#!/bin/sh

#
#  makeheader.sh ... ヘッダファイルをまとめる
#

(

cat <<EOF
/**
 **  cprime.h ... CPrime コンパイラ作成用ライブラリ
 **/

#ifndef _CPRIME_H_INCLUDED_
#define _CPRIME_H_INCLUDED_

EOF

for file in util.h scanner.h code.h; do
    sed -n 2p $file | sed 's/^.*\.\.\. \(.*\)$/\/*========== \1 ==========*\//'
    sed '1,6d;$d' $file
done

cat <<EOF
#endif /* _CPRIME_H_INCLUDED_ */
EOF

) > cprime.h
