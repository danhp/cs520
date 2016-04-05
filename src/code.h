#ifndef CODE_H
#define CODE_H

#define SEMICOLON print(";");
#define SPACE     print(" ");
#define NEWLINE   print("\n");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "pretty.h"

void codePROGRAM(PROGRAM *obj, int indentation);
void codeCPPheader();
void codePACKAGE(PACKAGE *obj, int indentation);
void codeTOP_DECL(TOP_DECL *obj, int indentation);
void codeVAR_DECL(VAR_DECL *obj, int indentation);
void codeTYPE_DECL(TYPE_DECL *obj, int indentation);
void codeID(ID *obj);
void codeFUNC_DECL(FUNC_DECL *obj, int indentation);
void codeTYPE(TYPE *obj);
void codeEXP(EXP *obj);
void codeEXPsingle(EXP *obj);
void codeSTRUCT_DECL(STRUCT_DECL *obj);
void codeFUNC_SIGN(FUNC_SIGN *obj, ID *id);

void codeSTMT(STMT *obj, int indentation, int isInsideLoop);
void codeSTMTprint(EXP *exp, int indentation);
void codeSTMTprintln(EXP *exp, int indentation);
void codeSTMTassign(EXP *left, EXP *right, int indentation);

void codeSYMBOL(SYMBOL *obj);
void codeFUNC_ARG(FUNC_ARG *obj);
void codeCASE_DECL(CASE_DECL *obj, int indentation);
void codeFOR_CLAUSE(FOR_CLAUSE *obj);

#endif
