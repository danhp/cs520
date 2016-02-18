#ifndef treeTREE_H
#define treeTREE_H

#include "tree.h"


void treePROGRAM(PROGRAM *obj, int indentation);
void treePACKAGE(PACKAGE *obj, int indentation);
void treeTOP_DECL(TOP_DECL *obj, int indentation);
void treeVAR_DECL(VAR_DECL *obj, int indentation);
void treeTYPE_DECL(TYPE_DECL *obj, int indentation);
void treeID(ID *obj, int indentation);
void treeFUNC_DECL(FUNC_DECL *obj, int indentation);
void treeTYPE(TYPE *obj, int indentation);
void treeEXP(EXP *obj, int indentation);
void treeSTRUCT_DECL(STRUCT_DECL *obj, int indentation);
void treeFUNC_SIGN(FUNC_SIGN *obj, int indentation);
void treeSTMT(STMT *obj, int indentation);
void treeSYMBOL(SYMBOL *obj, int indentation);
void treeFUNC_ARG(FUNC_ARG *obj, int indentation);
void treeCASE_DECL(CASE_DECL *obj, int indentation);
void treeFOR_CLAUSE(FOR_CLAUSE *obj, int indentation);

#endif
