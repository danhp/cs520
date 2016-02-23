#ifndef WEED_H
#define WEED_H

#define true 1
#define false 0

#include <stdlib.h>
#include "tree.h"
#include "error.h"

void weedPROGRAM(PROGRAM *);

void weedTOP_DECL(TOP_DECL *decl);

void weedTOPvar(VAR_DECL *decl);
void weedTOPtype(TYPE_DECL *decl);
void weedTOPfunc(FUNC_DECL *func);

void weedVARdecl(ID *id, EXP *exp, YYLTYPE loc);  /* #id = #exp */

void weedFUNC_SIGN(FUNC_SIGN *signature);

void weedSTMT(STMT *stmt);
void weedSTMTswitch(CASE_DECL *body);

void weedEXP(EXP *exp);
void weedEXPdivzero(EXP *exp);
void weedEXPfunccall(EXP *exp);

#endif

