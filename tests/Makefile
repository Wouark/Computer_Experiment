#
# Makefile ... コンパイルに使うファイルの依存関係
#

# make によるコンパイル
#
# make         ... 更新したファイルだけをコンパイルし，テストを実行
# make clean   ... 作られたファイルを削除

# コンパイラの指定

CC      = gcc
CFLAGS  = -Wall -W              # 基本的な警告を表示
#CFLAGS += -ansi -pedantic      # 行頭の '#' を消せば ISO 規格準拠の警告を表示
#CFLAGS += -O2                  # 行頭の '#' を消せば データ流解析で誤り検査

# ファイルを作る規則

ALLEXE = readtokens gencode compileseq rewriteaddress

all: $(ALLEXE) results.txt

.h.c:
	touch $*.c

.c.o:
	$(CC) $(CFLAGS) -c $<

readtokens: readtokens.c libcprime.a
	$(CC) $(CFLAGS) $^ -o $@

gencode: gencode.c libcprime.a
	$(CC) $(CFLAGS) $^ -o $@

compileseq: compileseq.c libcprime.a
	$(CC) $(CFLAGS) $^ -o $@

rewriteaddress: rewriteaddress.c libcprime.a
	$(CC) $(CFLAGS) $^ -o $@

results.txt: $(ALLEXE)
	./tests.sh > $@

# ファイルを消す規則

clean:
	rm -f $(ALLEXE) results.txt
