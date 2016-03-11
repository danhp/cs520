#ifndef SYMBOLTABLE
#define SYMBOLTABLE
#include "myRedBlackTree.h"

typedef struct LAYERMANAGER
{
  struct REDBLACKTREE *tree;
  struct LAYERMANAGER *next;

} LAYERMANAGER;


LAYERMANAGER *LAYER_newLayer(LAYERMANAGER *current);
int LAYER_addVariable(LAYERMANAGER *layer, char *name,  VARIABLETYPE varType);
VARIABLETYPE LAYER_varType(LAYERMANAGER *layer, char *name);
int LAYER_contains(LAYERMANAGER *layer, char *name);
LAYERMANAGER *LAYER_removeLayer(LAYERMANAGER *current);
void LAYER_printAllTrees(LAYERMANAGER *layer);
int LAYER_addVariableWithSubType(LAYERMANAGER *layer, char *name,  VARIABLETYPE varType, char *subType);
REDBLACKNODE *LAYER_getNode(LAYERMANAGER *layer, char *name);


#endif
