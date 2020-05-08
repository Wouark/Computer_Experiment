#!/bin/zsh

echo "samplesディレクトリ内のファイルを入力として自動で./compileを実行します"
echo "*.cprをコンパイルした結果を*.pcoに書き込みます"
echo "0~6のどのディレクトリのファイルを入力として実行しますか"
read num
for f in ../samples${num}/*.cpr; do
    ./compile $f > ${f%.*}.pco
    #echo ${${f##*/}%.*}
done
