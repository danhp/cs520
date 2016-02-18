#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pretty.h"
#include "tree.h"

extern FILE *prettyFile;

void print(char *text)
{
  fprintf(prettyFile, "%s", text);
  //printf("%s", text);
}

void printInteger(int number)
{
  fprintf(prettyFile, "%d", number);
  //printf("%d", number);
}

void printFloat(float number)
{
  fprintf(prettyFile, "%f", number);
  //printf("%f",number);
}

void printIndentation(int i)
{
  int j=0;
  for(;j<i;j++)
  {
    print("  ");
  }
}

void prettyPROGRAM(PROGRAM *obj, int indentation)
{
  printIndentation(indentation);
  prettyPACKAGE(obj->package, indentation);
  print("\n");
  prettyTOP_DECL(obj->top_decl, indentation);
}

void prettyPACKAGE(PACKAGE *obj, int indentation)
{
  printIndentation(indentation);

  print("package ");
  print(obj->name);
  print("\n");
}

void prettyTOP_DECL(TOP_DECL *obj, int indentation)
{
  if(obj->next != NULL)
  {
    prettyTOP_DECL(obj->next, indentation);
  }

  printIndentation(indentation);

  switch(obj->kind)
  {
    case top_varK:
      prettyVAR_DECL(obj->val.varT, indentation);
      break;
    case top_typeK:
      if(obj->val.typeT != NULL)
      {
        prettyTYPE_DECL(obj->val.typeT, indentation);
      }
      break;
    case top_funcK:
      prettyFUNC_DECL(obj->val.funcT, indentation);
      break;
  }
}

void prettyVAR_DECL(VAR_DECL *obj, int indentation)
{
  if(obj == NULL)
  {
    return;
  }

  if(obj->next != NULL)
  {
    prettyVAR_DECL(obj->next, indentation);
  }


  printIndentation(indentation);
  print("var ");
  prettyID(obj->id);
  print(" ");

  prettyTYPE(obj->type);
  
  if(obj->exp != NULL)
  {
    print(" = ");
    prettyEXP(obj->exp);
  }
  print("\n");
}

void prettyTYPE_DECL(TYPE_DECL *obj, int indentation)
{
  if(obj->next != NULL)
  {
    prettyTYPE_DECL(obj->next, indentation);
  }

  printIndentation(indentation);
  print("type ");
  print(obj->name);
  print(" ");

  prettyTYPE(obj->type);  
  print("\n");
}

void prettyID(ID *obj)
{
  if(obj->next != NULL)
  {
    prettyID(obj->next);
    print(", ");
  }

  print(obj->name);
}

void prettyFUNC_DECL(FUNC_DECL *obj, int indentation)
{
  printIndentation(indentation);

  print("func ");
  print(obj->name);
  prettyFUNC_SIGN(obj->signature);
  print("\n"); 
  printIndentation(indentation);
  print("{\n");
  prettySTMT(obj->body, indentation+1);
  print("}\n\n");
}

void prettyTYPE(TYPE *obj)
{
  if(obj == NULL)
  {
    return;
  }

  switch(obj->kind)
  {
    case type_refK:
      print(obj->name);
      return;
    case type_intK:
      print("int");
      return;
    case type_floatK:
      print("float64");
      break;
    case type_boolK:
      print("bool");
      break;
    case type_runeK:
      print("rune");
      break;
    case type_stringK:
      print("string");
      return;
    case type_arrayK:
      print("[");
      printInteger(obj->val.arrayT.size);
      print("] ");
      if(obj->val.arrayT.type != NULL)
      {
        prettyTYPE(obj->val.arrayT.type);
      }
      break;
    case type_sliceK:
      print("[] ");
      if(obj->val.sliceT.type != NULL)
      {
        prettyTYPE(obj->val.sliceT.type);
      }
      break;
    case type_structK:
      print("struct {");
      if(obj->val.structT.struct_decl != NULL)
      {
        prettySTRUCT_DECL(obj->val.structT.struct_decl);
      }
      print("}\n");
      return;
  }
}

void prettyEXP(EXP *obj)
{
  if(obj->next)
  {
    prettyEXP(obj->next);
    print(", ");
  }

  switch(obj->kind)
  {
    case idK:
      prettySYMBOL(obj->val.idE.sym);
      break;
    case intconstK:
      printInteger(obj->val.intconstE);
      break;
    case floatconstK:
     printFloat(obj->val.floatconstE);
      break;
    case boolconstK:
      if(obj->val.boolconstE)
      {
        print("true");
      }
      else
      {
        print("false");
      }
      break;
    case stringconstK:
      print(obj->val.stringconstE);
      break;
    case rawstringconstK:
      print(obj->val.rawstringconstE);
      break;
    case plusK:
      prettyEXP(obj->val.binaryE.left);
      print(" + ");
      prettyEXP(obj->val.binaryE.right);
      break;
    case minusK:
      prettyEXP(obj->val.binaryE.left);
      print(" - ");
      prettyEXP(obj->val.binaryE.right);
      break;
    case timesK:
      prettyEXP(obj->val.binaryE.left);
      print(" * ");
      prettyEXP(obj->val.binaryE.right);
      break;
    case divK:
      prettyEXP(obj->val.binaryE.left);
      print(" / ");
      prettyEXP(obj->val.binaryE.right);
      break;
    case modK:
      print(" % ");
      break;
    case bitandK:
      print(" & ");
      break;
    case andnotK:
      print(" &^ ");
      break;
    case bitorK:
      print("|");
      break;
    case bitxork:
      print(" ^ ");
      break;
    case leftshiftK:
      print(" << ");
      break;
    case rightshiftK:
      print(" >> ");
      break;
    case eqK:
      print(" == ");
      break;
    case neqK:
      print(" != ");
      break;
    case leK:
      print(" < ");
      break;
    case leqK:
      print(" <= ");
      break;
    case geK:
      print(" > ");
      break;
    case geqK:
      print(" >= ");
      break;
    case orK:
      print(" || ");
      break;
    case andK:
      print(" && ");
      break;
    case unotK:
      print("!");
      break;
    case uplusK:
      print("+");
      break;
    case uminusK:
      print("-");
      prettyEXP(obj->val.unaryE);
      break;
    case ubitnotK:
      print("^");
      break;
  }
}

void prettySTRUCT_DECL(STRUCT_DECL *obj)
{
  if(obj->next != NULL)
  {
    prettySTRUCT_DECL(obj->next);
    print(", ");
  }

  prettyID(obj->id);
  if(obj->type)
  {
    print(" ");
    prettyTYPE(obj->type);
  }
}

void prettyFUNC_SIGN(FUNC_SIGN *obj) 
{
  print("(");

  if(obj->arg)
  {
    prettyFUNC_ARG(obj->arg);
  }

  print(") ");
  if(obj->type != NULL)
  {
    prettyTYPE(obj->type);
  }
}

void prettySTMT(STMT *obj, int indentation)
{
  if(obj->next != NULL)
  {
    prettySTMT(obj->next, indentation);
  }


  switch(obj->kind)
  {
    case emptyK:
      break;
    case typeK:
      printIndentation(indentation);
      prettyTYPE_DECL(obj->val.typeS, indentation);
      break;
    case printK:
      printIndentation(indentation);
      print("print(");
      prettyEXP(obj->val.printS);
      print(")\n");
      break;
    case printlnK:
      printIndentation(indentation);
      print("println(");
      prettyEXP(obj->val.printlnS);
      break;
    case shortvarK:
      printIndentation(indentation);
      prettyEXP(obj->val.shortvarS.left);
      print(" := ");
      prettyEXP(obj->val.shortvarS.right);
      print("\n");
      break;
    case returnK:
      printIndentation(indentation);
      print("return ");
      prettyEXP(obj->val.returnS);
      print("\n");
      break;
    case ifK:
      printIndentation(indentation);
      print("if ");
      if(obj->val.ifS.pre_stmt != NULL)
      {
        prettySTMT(obj->val.ifS.pre_stmt, 0);
        print("; ");
      }
      print("(");
      prettyEXP(obj->val.ifS.condition);
      print(")\n");
      printIndentation(indentation);
      print("{\n");
      
      prettySTMT(obj->val.ifS.body, indentation + 1);

      printIndentation(indentation);
      print("}\n\n");
      break;
    case ifelseK:
      printIndentation(indentation);
      print("if ");
      if(obj->val.ifelseS.pre_stmt != NULL)
      {
        prettySTMT(obj->val.ifelseS.pre_stmt, 0);
        print("; ");
      }
      print("(");
      prettyEXP(obj->val.ifelseS.condition);
      print(")\n");
      printIndentation(indentation);
      print("{\n");
      
      prettySTMT(obj->val.ifelseS.thenpart, indentation + 1);

      printIndentation(indentation);
      print("}\n");
      printIndentation(indentation);
      print("else\n");
      printIndentation(indentation);
      print("{\n");

      prettySTMT(obj->val.ifelseS.elsepart, indentation+1);

      printIndentation(indentation);
      print("}\n\n");
      break;
    case switchK:
      printIndentation(indentation);
      print("switch ");
      if(obj->val.switchS.pre_stmt != NULL)
      {
        prettySTMT(obj->val.switchS.pre_stmt, 0);
        print("; ");
      }

      if(obj->val.switchS.condition != NULL)
      {
        prettyEXP(obj->val.switchS.condition);
      }
      print("\n");
      printIndentation(indentation);
      print("{\n");

      if(obj->val.switchS.body != NULL)
      {
        prettyCASE_DECL(obj->val.switchS.body, indentation+1);
      }

      printIndentation(indentation);
      print("}\n\n");
      break;
    case forK:
      printIndentation(indentation);
      prettyFOR_CLAUSE(obj->val.forS.for_clause);
      printIndentation(indentation);
      print("{\n");
      
      prettySTMT(obj->val.forS.body, indentation+1);
      
      printIndentation(indentation);
      print("}\n\n");
      break;
    case breakK:
      printIndentation(indentation);
      print("break;\n");
      break;
    case continueK:
      printIndentation(indentation);
      print("continue\n");
      break;
    case assignK:
      printIndentation(indentation);
      prettyEXP(obj->val.assignS.left);
      print(" = ");
      prettyEXP(obj->val.assignS.right);
      break;
    case expK:
      printIndentation(indentation);
      prettyEXP(obj->val.expS);
      break;
  }
}

void prettySYMBOL(SYMBOL *obj)
{
  print(obj->id);

  if(obj->next)
  {
    print(", ");
    prettySYMBOL(obj->next);
  }
}

void prettyFUNC_ARG(FUNC_ARG *obj)
{

  if(obj->next != NULL)
  {
    prettyFUNC_ARG(obj->next);
    print(", ");
  }

    if(obj->id != NULL)
    {
      prettyID(obj->id);
    }

    print(" ");
    prettyTYPE(obj->type);
}

void prettyCASE_DECL(CASE_DECL *obj, int indentation)
{
  if(obj->next != NULL)
  {
    prettyCASE_DECL(obj->next, indentation);
  }

  printIndentation(indentation);

  switch(obj->kind)
  {
    case caseK:
      print("case ");
      prettyEXP(obj->val.caseC.condition);
      print(":\n");
      prettySTMT(obj->val.caseC.stmt, indentation+1);
      break;
    case defaultK:
      print("default:\n");
      prettySTMT(obj->val.defaultC, indentation+1);
      break;
  }
}

void prettyFOR_CLAUSE(FOR_CLAUSE *obj)
{
  print("for ");
  if(obj->init_stmt != NULL)
  {
    prettySTMT(obj->init_stmt, 0);
  }
  print("; ");

  if(obj->condition != NULL)
  {
    prettyEXP(obj->condition);
  }
  print("; ");

  if(obj->post_stmt != NULL)
  {
    prettySTMT(obj->post_stmt, 0);
  }
}

