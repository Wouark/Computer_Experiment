#
#  Makefile ... ファイル間の依存関係
#

CC = gcc
#CC = g++
CFLAGS = -Wall -W -ansi -pedantic

# ファイルを作る規則

all: run libcprime.a cprime.h

run: util.o code.o run.o
#run: run.o libcprime.a
	$(CC) $(CFLAGS) $^ -o $@
	chmod a+rxw $@

libcprime.a: util.o scanner.o code.o
	$(AR) rs $@ $^
	chmod a+r $@

cprime.h: makeheader.sh
	./makeheader.sh
	chmod a+r $@

.h.c:
	touch $*.c

.c.o:
	$(CC) $(CFLAGS) -c $<

# ファイルを消す規則

cleanall: clean
	$(RM) libcprime.a run cprime.h

clean:
	$(RM) util.o scanner.o code.o run.o
