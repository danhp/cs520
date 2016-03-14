#include <stdlib.h>
#include "redblacktree.h"
#include "table.h"

SYMBOL *blankSymbol;

SymbolTable *initSymbolTable()
{
  REDBLACKTREE *newTree = malloc(sizeof(REDBLACKTREE));
  SymbolTable *t = malloc(sizeof(SymbolTable));
  t->tree = newTree;
  t->next = NULL;
  return t;
}

SymbolTable *scopeSymbolTable(SymbolTable *s)
{
  SymbolTable *t = malloc(sizeof(SymbolTable));
  t = initSymbolTable();
  t->next = s;
  return t;
}

SymbolTable *unscopeSymbolTable(SymbolTable *previous)
{
  return previous->next;
}

SYMBOL *putSymbol(SymbolTable *t, char *name, SymbolKind kind, YYLTYPE loc)
{
  return TREE_addVariable(t->tree, name, kind, loc);
}

SYMBOL *putSymbolDebug(SymbolTable *t, char *name, SymbolKind kind)
{
  return TREE_addVariableDebug(t->tree, name, kind);
}

SYMBOL *getSymbol(SymbolTable *t, char *name)
{
  SYMBOL *node = TREE_findNode(t->tree, name);
  if(node == NULL)
  {
    if(t->next == NULL)
    {
      return NULL;
    }
    else
    {
      return getSymbol(t->next, name);
    }
  }
  else
  {
    return node;
  }
}

int defSymbol(SymbolTable *t, char *name)
{
  SYMBOL *node = getSymbol(t, name);
  if(node==NULL)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

SYMBOL *blank()
{
  if(blankSymbol == NULL)
  {
    blankSymbol = malloc(sizeof(SYMBOL));
    blankSymbol->id  = "_";
  }

  return blankSymbol;
}

void printSymbolTable(SymbolTable *t)
{
  TREE_printTree(t->tree);
  if(t->next != NULL)
  {
    printSymbolTable(t->next);
  }

}
