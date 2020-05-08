#!/bin/sh

#
# tests.sh ... 参考プログラムのテスト
#

echo '===== readtokens ====='
file='../../CPrime/samples0/tokens.cpr'
cat $file
./readtokens $file

echo '===== gencode ====='
./gencode

echo '===== compileseq ====='
input='0, 1, 2,\n3, 4, 5\n'
printf "$input"
printf "$input" | ./compileseq
printf "$input" | ./compileseq | ./run

echo '===== rewriteaddress ====='
./rewriteaddress
