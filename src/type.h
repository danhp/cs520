#ifndef TYPEH
#define TYPEH

#include <string.h>

#include "tree.h"
#include "error.h"
#include "symbol.h"
#include "memory.h"

#define true 1
#define false 0

void typePROGRAM(PROGRAM *p);
void typePACKAGE(PACKAGE *p);

void typeTOP_DECL(TOP_DECL *decl);
void typeVAR_DECL(VAR_DECL *decl);
void typeTYPE_DECL(TYPE_DECL *decl);
void typeSTRUCT_DECL(STRUCT_DECL *decl);

void typeTYPE(TYPE *type);

void typeFUNC_DECL(FUNC_DECL *decl);
void typeFUNC_SIGN(FUNC_SIGN *sign);
void typeFUNC_ARG(FUNC_ARG *arg);
void typeReturnsSTMT(STMT *stmt, TYPE *type);
void typeReturnsCASE(CASE_DECL *decl, TYPE *type);

void typeSTMT(STMT *stmt);
void typeCASE_DECL(CASE_DECL *decl, TYPE *typeCondition);
void typeFOR_CLAUSE(FOR_CLAUSE *decl);

TYPE *typeEXP(EXP *decl);
TYPE *typeEXPplus(EXP *left, EXP *right);
TYPE *typeEXPnumeric(EXP *left, EXP *right);
TYPE *typeEXPbit(EXP *left, EXP *right);
TYPE *typeEXPcomparable(EXP *left, EXP *right);
TYPE *typeEXPordered(EXP *left, EXP *right);
TYPE *typeEXPbool(EXP *left, EXP *right);
TYPE *typeEXPunumeric(EXP *exp);
TYPE *typeEXPubool(EXP *exp);
TYPE *typeEXPuint(EXP *exp);

TYPE *typeEXPselector(EXP *exp, ID *id);
TYPE *typeEXPindex(EXP *exp, EXP *index);
TYPE *typeEXPappend(ID *id, EXP *exp);
TYPE *typeEXPfunccall(EXP *exp);
TYPE *typeEXPcast(TYPE *type, EXP *exp);

void typeSHORTVAR(EXP *left, EXP *right);
void typeAssign(EXP *left, EXP *right);

void checkEqual(TYPE *t1, TYPE *t2, YYLTYPE loc);
int isSameType(TYPE *t1, TYPE *t2);
int isSameStruct(STRUCT_DECL *d1, STRUCT_DECL *d2);
void checkAssignable(TYPE *left, TYPE *right, YYLTYPE loc);
int isTypeAssignble(TYPE *left, TYPE *right);
void checkCastable(TYPE *left, TYPE *right, YYLTYPE loc);
int isTypeCastable(TYPE *left, TYPE *right);
int isSameLength(FUNC_ARG *arg, EXP *exp);
int isPrintableType(EXP *exp);

#endif
