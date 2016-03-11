#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myRedBlackTree.h"
#include "SymbolTable.h"

int main(void)
{
  LAYERMANAGER *layerManager = LAYER_newLayer(NULL);

  if(LAYER_addVariable(layerManager, "test1", VariableType_int)){printf("true\n");}else{printf("false\n");}
  if(LAYER_addVariable(layerManager, "test1", VariableType_int)){printf("true\n");}else{printf("false\n");}
  if(LAYER_addVariable(layerManager, "test2", VariableType_int)){printf("true\n");}else{printf("false\n");}

  if(LAYER_varType(layerManager, "test2") == VariableType_DNE){printf("false\n");}else{printf("true\n");}
  printf("--------\n");

  layerManager = LAYER_newLayer(layerManager);

  if(LAYER_addVariable(layerManager, "test1", VariableType_int)){printf("true\n");}else{printf("false\n");}
  if(LAYER_addVariable(layerManager, "test1", VariableType_int)){printf("true\n");}else{printf("false\n");}

  printf("--------\n");

  if(LAYER_varType(layerManager, "test2") == VariableType_DNE){printf("false\n");}else{printf("true\n");}
  if(LAYER_varType(layerManager, "test3") == VariableType_DNE){printf("false\n");}else{printf("true\n");}
  printf("%d\n",LAYER_contains(layerManager, "test2"));
  printf("%d\n",LAYER_contains(layerManager, "test3"));

  printf("--------\n");
  LAYER_printAllTrees(layerManager);
  printf("---> The top layer was removed <---\n");
  layerManager = LAYER_removeLayer(layerManager);
  LAYER_printAllTrees(layerManager);

  printf("done\n");
}
