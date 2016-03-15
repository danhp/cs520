#ifndef SYMBOLH
#define SYMBOLH

#include "tree.h"
#include "error.h"
#include "memory.h"
#include "redblacktree.h"
#include "table.h"

void symPROGRAM(PROGRAM *obj, int print);
void symTOP_DECL(TOP_DECL *obj);
void symVAR_DECL(VAR_DECL *obj);
void symTYPE_DECL(TYPE_DECL *obj);
void symSTRUCT_DECL(STRUCT_DECL *obj);
void symTYPE(TYPE *obj);
void symFUNC_DECL(FUNC_DECL *obj);
void symFUNC_SIGN(FUNC_SIGN *obj);
void symFUNC_ARG(FUNC_ARG *obj);
void symSTMT(STMT *obj);
void symCASE_DECL(CASE_DECL *obj);
void symFOR_CLAUSE(FOR_CLAUSE *obj);
void symEXP(EXP *obj);
void symSHORTVAR(EXP *obj);

#endif
