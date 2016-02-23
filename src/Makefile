CC = gcc
CFLAGS = -Wall -std=c99 -pedantic

FILES = error memory weed tree pretty
O_FILES = $(addsuffix .o, $(FILES))
H_FILES = $(addsuffix .h, $(FILES))

main: y.tab.o lex.yy.o main.o $(H_FILES) $(O_FILES)
	$(CC) lex.yy.o y.tab.o main.o $(O_FILES) -o golite -ll

y.tab.c y.tab.h: golite.y
	bison --yacc --defines golite.y

lex.yy.c: golite.l y.tab.h
	flex golite.l

test: main
	./test.sh

clean:
	rm y.tab.* lex.yy.*; rm *.o; rm golite

