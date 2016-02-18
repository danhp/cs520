#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prettyTree.h"
#include "tree.h"

extern FILE *prettyTree;

void printText(char *text)
{
  fprintf(prettyTree, "%s", text);
}

void printInt(int number)
{
  fprintf(prettyTree, "%d", number);
}

void printFl(float number)
{
  fprintf(prettyTree, "%f", number);
}

void printInd(int i)
{
  int j=0;
  for(;j<i;j++)
  {
    printText("  ");
  }
}


void treePROGRAM(PROGRAM *obj, int indentation)
{
  printText("PROGRAM\n");
  printText("{\n");
  printInd(indentation+1);
  printText("struct PACKAGE *package\n");
  printInd(indentation+1);
  printText("{\n");
  treePACKAGE(obj->package, indentation+2);
  printInd(indentation+1);
  printText("}\n");
  printInd(indentation+1);
  printText("struct TOP_DECL *top_decl\n");
  printInd(indentation+1);
  printText("{\n");
  treeTOP_DECL(obj->top_decl, indentation+2);
  printInd(indentation+1);
  printText("}\n");
  printText("}\n");
}

void treePACKAGE(PACKAGE *obj, int indentation)
{
  printInd(indentation);
  printText("char *name = ");
  printText(obj->name);
  printText("\n");
}


void treeTOP_DECL(TOP_DECL *obj, int indentation)
{
  printInd(indentation);
  printText("enum kind = ");
  switch(obj->kind)
  {
    case top_varK:
      printText("top_varK\n");
      printInd(indentation);
      printText("struct VAR_DECL *varT\n");
      printInd(indentation);
      printText("{\n");
      treeVAR_DECL(obj->val.varT,indentation+1);
      printInd(indentation);
      printText("}\n");
      break;
    case top_typeK:
      printText("top_typeK\n");
      printInd(indentation);
      printText("struct TYPE_DECL *typeT");
      if(obj->val.typeT != NULL)
      {
        printText("\n");
        printInd(indentation);
        printText("{\n");
        treeTYPE_DECL(obj->val.typeT, indentation+1);
        printInd(indentation);
        printText("}\n");
      }
      else
      {
        printText(" = NULL\n");
      }
      break;
    case top_funcK:
      printText("top_funcK\n");
      printInd(indentation);
      printText("struct FUN_DECL *funcT\n");
      printInd(indentation);
      printText("{\n");
      treeFUNC_DECL(obj->val.funcT, indentation+1);
      printInd(indentation);
      printText("}\n");
      break;
  }

  printInd(indentation);
  printText("struct TOP_DECL *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTOP_DECL(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeVAR_DECL(VAR_DECL *obj, int indentation)
{
  printInd(indentation);
}


void treeTYPE_DECL(TYPE_DECL *obj, int indentation)
{
  printInd(indentation);
  printText("char *name = ");
  if(obj->name != NULL)
  {
    printText(obj->name);
    printText("\n");
  }
  else
  {
    printText("NULL\n");
  }
  printInd(indentation);
  printText("struct TYPE *type");

  if(obj->type != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE(obj->type, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("struct TYPE_DECL *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE_DECL(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeID(ID *obj, int indentation)
{
  printInd(indentation);
  printText("char *name = ");
  printText(obj->name);
  printText("\n");
  printInd(indentation);
  printText("struct ID *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeID(obj->next, indentation + 1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  } 
}


void treeFUNC_DECL(FUNC_DECL *obj, int indentation)
{
  printInd(indentation);
  printText("char *name = ");
  printText(obj->name);
  printText("\n");
  printInd(indentation);
  printText("struct FUNC_SIGN *signature\n");
  printInd(indentation);
  printText("{\n");
  treeFUNC_SIGN(obj->signature,indentation+1);
  printInd(indentation);
  printText("}\n");

  printInd(indentation);
  printText("struct STMT *body\n");
  printInd(indentation);
  printText("{\n");
  treeSTMT(obj->body,indentation+1);
  printInd(indentation);
  printText("}\n");
}


void treeTYPE(TYPE *obj, int indentation)
{
  printInd(indentation);
  printText("char *name = ");
  
  if(obj->name != NULL)
  {
    printText(obj->name);
  }
  else
  {
    printText("NULL");
  }

  printText("\n");
  printInd(indentation);
  printText("enum kind = ");

  switch(obj->kind)
  {
    case type_refK:
      printText("type_refK\n");
      break;
    case type_intK:
      printText("type_intK\n");
      break;
    case type_floatK:
      printText("type_floatK\n");
      break;
    case type_boolK:
      printText("type_boolK\n");
      break;
    case type_runeK:
      printText("type_runeK\n");
      break;
    case type_stringK:
      printText("type_stringK\n");
      break;
    case type_arrayK:
      printText("type_arrayK\n");
      printInd(indentation);
      printText("int size = ");
      printInt(obj->val.arrayT.size);
      printText("\n");
      printInd(indentation);
      printText("struct TYPE *type");
      if(obj->val.arrayT.type != NULL)
      {
        printText("\n");
        printInd(indentation);
        printText("{\n");
        treeTYPE(obj->val.arrayT.type, indentation);
        printInd(indentation);
        printText("}\n");
      }
      else
      {
        printText(" = NULL\n");
      }
      break;
    case type_sliceK:
      printText("type_sliceK\n");
      break;
    case type_structK:
      printText("type_structK\n");
      printInd(indentation);
      printText("struct strcutT\n");
      printInd(indentation);
      printText("{\n");
      printInd(indentation+1);
      printText("struct STRUCT_DECL *struct_decl");
      if(obj->val.structT.struct_decl != NULL)
      {
        printText("\n");
        printInd(indentation+1);
        printText("{\n");
        treeSTRUCT_DECL(obj->val.structT.struct_decl, indentation+2);
        printInd(indentation+1);
        printText("}\n");
      }
      else
      {
        printText(" = NULL\n");
      }
      printInd(indentation);
      printText("}\n");
      break;
  }
}


void treeEXP(EXP *obj, int indentation)
{
  printInd(indentation);
  printText("struct TYPE *type");
  if(obj->type != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE(obj->type, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("enum kind = ");
  switch(obj->kind)
  {
    case idK:
      printText("idK\n");
      break;
    case intconstK:
      printText("intconstK\n");
      break;
    case floatconstK:
      printText("floatconstK\n");
      break;
    case boolconstK:
      printText("boolconstK\n");
      break;
    case stringconstK:
      printText("stringconstK\n");
      break;
    case rawstringconstK:
      printText("rawstringconstK\n");
      break;
    case plusK:
      printText("plusK\n");
      break;
    case minusK:
      printText("minusK\n");
      break;
    case timesK:
      printText("timesK\n");
      break;
    case divK:
      printText("divK\n");
      break;
    case modK:
      printText("modK\n");
      break;
    case bitandK:
      printText("bitandK\n");
      break;
    case andnotK:
      printText("andnotK\n");
      break;
    case bitorK:
      printText("bitorK\n");
      break;
    case bitxork:
      printText("bitxork\n");
      break;
    case leftshiftK:
      printText("leftshiftK\n");
      break;
    case rightshiftK:
      printText("rightshiftK\n");
      break;
    case eqK:
      printText("eqK\n");
      break;
    case neqK:
      printText("neqK\n");
      break;
    case leK:
      printText("leK\n");
      break;
    case leqK:
      printText("leqK\n");
      break;
    case geK:
      printText("geK\n");
      break;
    case geqK:
      printText("geqK\n");
      break;
    case orK:
      printText("orK\n");
      break;
    case andK:
      printText("andK\n");
      break;
    case uplusK:
      printText("uplusK\n");
      break;
    case uminusK:
      printText("uminusK\n");
      break;
    case unotK:
      printText("unotK\n");
      break;
    case ubitnotK:
      printText("ubitnotK\n");
      break;
    case selectorK:
      printText("selectorK\n");
      break;
    case indexK:
      printText("indexK\n");
      break;
    case funccallK:
      printText("funccallK\n");
      break;
    case appendK:
      printText("appendK\n");
      break;
    case castK:
      printText("castK\n");
      break;
  }

  printInd(indentation);
  printText("struct EXP *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeEXP(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeSTRUCT_DECL(STRUCT_DECL *obj, int indentation)
{
  printInd(indentation);
  printText("struct ID *id");
  if(obj->id != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeID(obj->id, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("struct TYPE *type");
  if(obj->type != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE(obj->type, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("struct STRUCT_DECL *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeSTRUCT_DECL(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeFUNC_SIGN(FUNC_SIGN *obj, int indentation)
{
  printInd(indentation);
  printText("struct FUNC_ARG *arg");
  if(obj->arg != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeFUNC_ARG(obj->arg, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("struct TYPE *type");
  if(obj->type != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE(obj->type,indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeSTMT(STMT *obj, int indentation)
{
  printInd(indentation);
  printText("enum kind = ");
  switch(obj->kind)
  {
    case emptyK:
      printText("emptyK\n");
      break;
    case varK:
      printText("varK\n");
      break;
    case typeK:
      printText("typeK\n");
      break;
    case printK:
      printText("printK\n");
      break;
    case printlnK:
      printText("printlnK\n");
      break;
    case shortvarK:
      printText("shortvarK\n");
      break;
    case returnK:
      printText("returnK\n");
      break;
    case ifK:
      printText("ifK\n");
      break;
    case ifelseK:
      printText("ifelseK\n");
      break;
    case switchK:
      printText("switchK\n");
      printInd(indentation);
      printText("struct switchS\n");
      printInd(indentation);
      printText("{\n");
      printInd(indentation+1);
      printText("struct STMT *pre_stmt");
      if(obj->val.switchS.pre_stmt != NULL)
      {
        printText("\n");
        printInd(indentation+1);
        printText("{\n");
        treeSTMT(obj->val.switchS.pre_stmt, indentation+2);
        printInd(indentation+1);
        printText("}\n");
      }
      else
      {
        printText(" = NULL\n");
      }
      printInd(indentation+1);
      printText("struct EXP *condition\n");
      printInd(indentation+1);
      printText("{\n");
      treeEXP(obj->val.switchS.condition, indentation+2);
      printInd(indentation+1);
      printText("}\n");
      printInd(indentation+1);
      printText("struct CASE_DECL *body");
      if(obj->val.switchS.body != NULL)
      {
        printText("\n");
        printInd(indentation+1);
        printText("{\n");
        treeCASE_DECL(obj->val.switchS.body, indentation+2);
        printInd(indentation+1);
        printText("}\n");
      }
      else
      {
        printText(" = NULL\n");
      }
      printInd(indentation);
      printText("}\n");
      break;
    case forK:
      printText("forK\n");
      break;
    case breakK:
      printText("breakK\n");
      break;
    case continueK:
      printText("continueK\n");
      break;
    case assignK:
      printText("assignK\n");
      break;
    case expK:
      printText("expK\n");
      break;
  }

  printInd(indentation);
  printText("struct STMT *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeSTMT(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeSYMBOL(SYMBOL *obj, int indentation)
{
  printInd(indentation);
}


void treeFUNC_ARG(FUNC_ARG *obj, int indentation)
{
  printInd(indentation);
  printText("struct ID *id");
  if(obj->id != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeID(obj->id, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

  printInd(indentation);
  printText("struct TYPE *type");
  if(obj->type != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeTYPE(obj->type,indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL");
  }

  printInd(indentation);
  printText("struct FUNC_ARG *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeFUNC_ARG(obj->next,indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }

}


void treeCASE_DECL(CASE_DECL *obj, int indentation)
{
  printInd(indentation);
  printText("enum kind = ");
  switch(obj->kind)
  {
    case caseK:
      printText("caseK\n");
      break;
    case defaultK:
      printText("defaultK\n");
      break;
  }
  printInd(indentation);
  printText("struct CASE_DECL *next");
  if(obj->next != NULL)
  {
    printText("\n");
    printInd(indentation);
    printText("{\n");
    treeCASE_DECL(obj->next, indentation+1);
    printInd(indentation);
    printText("}\n");
  }
  else
  {
    printText(" = NULL\n");
  }
}


void treeFOR_CLAUSE(FOR_CLAUSE *obj, int indentation)
{
  printInd(indentation);
}

