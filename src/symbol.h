#ifndef SYMBOLH
#define SYMBOLH

#include "tree.h"
#include "error.h"
#include "redblacktree.h"
#include "table.h"


/* Returns the SymbolKind associated with TYPE `type` */
SymbolKind symKindFromTYPE(TYPE *type);

void symPROGRAM(PROGRAM *obj, int print);
void symTOP_DECL(TOP_DECL *obj, SymbolTable *sym);
void symVAR_DECL(VAR_DECL *obj, SymbolTable *sym);
void symTYPE_DECL(TYPE_DECL *obj, SymbolTable *sym);
void symSTRUCT_DECL(STRUCT_DECL *obj, SymbolTable *sym);
void symTYPE(TYPE *obj, SymbolTable *sym);
void symFUNC_DECL(FUNC_DECL *obj, SymbolTable *sym);
void symFUNC_SIGN(FUNC_SIGN *obj, SymbolTable *sym);
void symFUNC_ARG(FUNC_ARG *obj, SymbolTable *sym);
void symSTMT(STMT *obj, SymbolTable *sym);
void symCASE_DECL(CASE_DECL *obj, SymbolTable *sym);
void symFOR_CLAUSE(FOR_CLAUSE *obj, SymbolTable *sym);
void symEXP(EXP *obj, SymbolTable *sym);
void symSHORTVAR(EXP *obj, SymbolTable *sym);

#endif
