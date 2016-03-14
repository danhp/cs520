#ifndef SYMBOL_H
#define SYMBOL_H

#include "redblacktree.h"

typedef struct SymbolTable
{
  struct REDBLACKTREE *tree;
  struct SymbolTable *next;
} SymbolTable;


SymbolTable *initSymbolTable();
SymbolTable *scopeSymbolTable(SymbolTable *s);
SymbolTable *unscopeSymbolTable(SymbolTable *previous);
SYMBOL *putSymbol(SymbolTable *t, char *name, SymbolKind kind, YYLTYPE loc);
SYMBOL *getSymbol(SymbolTable *t, char *name);
int defSymbol(SymbolTable *t, char *name);
SYMBOL *blank();
SYMBOL *putSymbolDebug(SymbolTable *t, char *name, SymbolKind kind);
void printSymbolTable(SymbolTable *t);

#endif
