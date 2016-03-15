#ifndef SYMBOL_H
#define SYMBOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "y.tab.h"
#include "error.h"
#include "redblacktree.h"

typedef struct SymbolTable
{
  struct REDBLACKTREE *tree;
  struct SymbolTable *next;
} SymbolTable;

int shouldPrint;
SymbolTable *sym;

SymbolTable *initSymbolTable();
SymbolTable *scopeSymbolTable(SymbolTable *s);
SymbolTable *unscopeSymbolTable(SymbolTable *s, int line);
SYMBOL *putSymbol(SymbolTable *t, char *name, TYPE *type, YYLTYPE loc);
SYMBOL *getSymbol(SymbolTable *t, char *name, YYLTYPE loc);
int isInTopFrame(SymbolTable *table, char *name);
SYMBOL *blank();

void printSymbolTable(SymbolTable *t);
void printFrame(SymbolTable *t, int line);

#endif
