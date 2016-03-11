#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myRedBlackTree.h"
#include "SymbolTable.h"

//adds a new layer on the current layer manager. Pass NULL to make a new one from scratch.
LAYERMANAGER *LAYER_newLayer(LAYERMANAGER *current)
{
  REDBLACKTREE *newTree = malloc(sizeof(REDBLACKTREE));

  LAYERMANAGER *layerManager = malloc(sizeof(LAYERMANAGER));
  layerManager->tree = newTree;
  layerManager->next = current;

  return layerManager;
}

LAYERMANAGER *LAYER_removeLayer(LAYERMANAGER *current)
{
  return current->next;
}

void LAYER_printAllTrees(LAYERMANAGER *layer)
{
  TREE_printTree(layer->tree);
  if(layer->next != NULL)
  {
    LAYER_printAllTrees(layer->next);
  }
}

int LAYER_addVariable(LAYERMANAGER *layer, char *name,  VARIABLETYPE varType)
{
  return TREE_addVariable(layer->tree, name, varType);
}

int LAYER_addVariableWithSubType(LAYERMANAGER *layer, char *name,  VARIABLETYPE varType, char *subType)
{
  return TREE_addVariableWithSubType(layer->tree, name, varType, subType);
}

VARIABLETYPE LAYER_varType(LAYERMANAGER *layer, char *name)
{
  REDBLACKNODE *node = TREE_findNode(layer->tree, name);
  if(node == NULL)
  {
    if(layer->next == NULL)
    {
      return VariableType_DNE;
    }
    else
    {
      return LAYER_varType(layer->next, name);
    }
  }
  else
  {
    return node->varType;
  }
}

int LAYER_contains(LAYERMANAGER *layer, char *name)
{
  REDBLACKNODE *node = TREE_findNode(layer->tree, name);
  if(node == NULL)
  {
    if(layer->next == NULL)
    {
      return 0;
    }
    else
    {
      return LAYER_varType(layer->next, name);
    }
  }
  else
  {
    return 1;
  }
}

REDBLACKNODE *LAYER_getNode(LAYERMANAGER *layer, char *name)
{
  return TREE_findNode(layer->tree, name);
}
