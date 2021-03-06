#ifndef PRETTY_H
#define PRETTY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

void print(char *text);

void prettyPROGRAM(PROGRAM *obj, int indentation, int pptype_flag);
void prettyPACKAGE(PACKAGE *obj, int indentation);
void prettyTOP_DECL(TOP_DECL *obj, int indentation);
void prettyVAR_DECL(VAR_DECL *obj, int indentation);
void prettyTYPE_DECL(TYPE_DECL *obj, int indentation);
void prettyID(ID *obj);
void prettyFUNC_DECL(FUNC_DECL *obj, int indentation);
void prettyTYPE(TYPE *obj);
void prettyEXP(EXP *obj);
void prettySTRUCT_DECL(STRUCT_DECL *obj);
void prettyFUNC_SIGN(FUNC_SIGN *obj);
void prettySTMT(STMT *obj, int indentation);
void prettySYMBOL(SYMBOL *obj);
void prettyFUNC_ARG(FUNC_ARG *obj);
void prettyCASE_DECL(CASE_DECL *obj, int indentation);
void prettyFOR_CLAUSE(FOR_CLAUSE *obj);

void printInteger(int number);
void printFloat(float number);
void printRune(char c);
void prettySYMBOL(SYMBOL *obj);
void prettyID(ID *obj);
void printIndentation(int i);

#endif
