#ifndef WEED_H
#define WEED_H

#define true 1
#define false 0

#include <stdlib.h>
#include "tree.h"
#include "error.h"

void weedPROGRAM(PROGRAM *p);

void weedPACKAGE(PACKAGE *p);

void weedTOP_DECL(TOP_DECL *decl);

void weedTOPvar(VAR_DECL *decl);
void weedTOPtype(TYPE_DECL *decl);
void weedTOPfunc(FUNC_DECL *func);

void weedVARdecl(ID *id, EXP *exp, YYLTYPE loc);  /* #id = #exp */

int weedFUNC_SIGN(FUNC_SIGN *signature);

void weedSTMT(STMT *stmt, int isInsideLoop, int isInsideSwitch);
int weedSTMTfuncreturn(STMT *stmt, int isValuedReturn);
int weedSTMTfuncreturnifelse(STMT *stmt, int isValuedReturn);
int weedSTMTfuncreturnfor(STMT *stmt, int isValuedReturn);
int weedSTMTfuncreturnswitch(CASE_DECL *body, int isValuedReturn);
void weedSTMTswitch(CASE_DECL *body, int isInsideLoop);
void weedSTMTshorvar(EXP *left, EXP *right);
void weedSTMTassign(EXP *left, EXP *right);
void weedSTMTexp(EXP *exp);

void weedFOR_CLAUSE(FOR_CLAUSE *clause);

void weedEXP(EXP *exp);
void weedEXPdivzero(EXP *exp);
void weedEXPcallId(EXP *exp);
void weedEXPlvalue(EXP *exp);

void weedIDblank(ID *id);

#endif
