#ifndef MYREDBLACKTREEHEADER
#define MYREDBLACKTREEHEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "tree.h"
#include "pretty.h"
#include "y.tab.h"


typedef struct REDBLACKTREE {
  struct SYMBOL *root;
} REDBLACKTREE;

SYMBOL *makeSYMBOL(char *id, SymbolKind kind, YYLTYPE loc);
int TREE_addSYMBOL( REDBLACKTREE *tree, SYMBOL *node);
SYMBOL *NODE_findNode(SYMBOL *node, char *id);
SYMBOL *TREE_findNode(REDBLACKTREE *tree, char *id);
int NODE_addNodeIfDoesNotExist(SYMBOL *node, SYMBOL *newNode);

void treeRotation(SYMBOL *node);
int isRed(SYMBOL *node);
int isBlack(SYMBOL *node);
void setBlack(SYMBOL *node);
void setRed(SYMBOL *node);

SYMBOL *TREE_addVariable(REDBLACKTREE *tree, char *id,  SymbolKind kind, YYLTYPE loc);
void TREE_printTree(REDBLACKTREE *tree);
void NODE_printNode(SYMBOL *node, int indentation);
void NODE_printIndentation(int indentation);
SYMBOL *TREE_addVariableDebug(REDBLACKTREE *tree, char *id,  SymbolKind kind);

void printSymbols(REDBLACKTREE *tree, int line);
void printSyms(SYMBOL *s);
void symbolTypeToString(SYMBOL *s);
char *typeToString(TYPE *t);

#endif
