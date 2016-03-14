#include "symbol.h"

void symPROGRAM(PROGRAM *obj)
{ SymbolTable *sym = initSymbolTable();

  // add true/false
  putSymbol(sym, "true", boolSym, yylloc);
  putSymbol(sym, "false", boolSym, yylloc);

  SymbolTable *localsym = scopeSymbolTable(sym);
  symTOP_DECL(obj->top_decl, localsym);
}

void symTOP_DECL(TOP_DECL *obj, SymbolTable *sym)
{ if (obj->next) symTOP_DECL(obj->next, sym);

  switch(obj->kind) {
    case top_varK:
      symVAR_DECL(obj->val.varT, sym);
      break;
    case top_typeK:
      symTYPE_DECL(obj->val.typeT, sym);
      break;
    case top_funcK:
      symFUNC_DECL(obj->val.funcT, sym);
      break;
  }
}

void symVAR_DECL(VAR_DECL *obj, SymbolTable *sym)
{ SymbolKind kind;

  if(!obj) return; if(obj->next) symVAR_DECL(obj->next, sym);

  if (obj->type) {  /* if type was declared, use it */
    kind = symKindFromTYPE(obj->type);
  } else {
    kind = inferredSym;
  }

  ID *cur = obj->id;
  while (cur) {
    SYMBOL *s = putSymbol(sym, cur->name, kind, obj->loc);
    cur->symbol = s;
    cur = cur->next;
  }
}

void symTYPE_DECL(TYPE_DECL *obj, SymbolTable *sym)
{ SYMBOL *s;

  if(!obj) return; if(obj->next) symTYPE_DECL(obj->next, sym);

  s = putSymbol(sym, obj->id->name, typeSym, obj->loc);
  s->val.type = obj->type;

  symTYPE(obj->type, sym);
}

void symTYPE(TYPE *obj, SymbolTable *sym)
{ if(!obj) return;
  switch(obj->kind) {
    case type_refK:
      obj->val.refT.id->symbol = getSymbol(sym, obj->val.refT.id->name);;
      return;
    case type_intK:
    case type_floatK:
    case type_boolK:
    case type_runeK:
    case type_stringK:
      break;
    case type_arrayK:
      if(obj->val.arrayT.type) {
        symTYPE(obj->val.arrayT.type, sym);
      }
      break;
    case type_sliceK:
      if(obj->val.sliceT.type) {
        symTYPE(obj->val.sliceT.type, sym);
      }
      break;
    case type_structK:
      if(obj->val.structT.struct_decl) {
        symSTRUCT_DECL(obj->val.structT.struct_decl, scopeSymbolTable(sym));
      }
      return;
  }
}

void symSTRUCT_DECL(STRUCT_DECL *obj, SymbolTable *sym)
{ SYMBOL *s;
  if(obj->next) { symSTRUCT_DECL(obj->next, sym); }

  s = putSymbol(sym, obj->id->name, structSym, obj->loc);
  s->val.structDecl = obj;
}

/* 1)Add function type to top-level symbol table
 * 2)Create local symbol table and add parameters to it
 */
void symFUNC_DECL(FUNC_DECL *obj, SymbolTable *sym)
{ SYMBOL *s;

  s = putSymbol(sym, obj->id->name, funcSym, obj->loc);
  s->val.funcSignature = obj->signature;
  obj->id->symbol = s;

  SymbolTable *localsym = scopeSymbolTable(sym);
  symFUNC_SIGN(obj->signature, localsym);
  symSTMT(obj->body, localsym);

  /* unscopeSymbolTable(localSym); */
}

void symFUNC_SIGN(FUNC_SIGN *obj, SymbolTable *sym)
{
  if (obj->arg)
    symFUNC_ARG(obj->arg, sym);
}

void symFUNC_ARG(FUNC_ARG *obj, SymbolTable *sym)
{ if (!obj) return;
  if(obj->next) { symFUNC_ARG(obj->next, sym); }

  ID *cur = obj->id;
  while (cur) {
    SYMBOL *s = putSymbol(sym, cur->name, symKindFromTYPE(obj->type), obj->loc);
    cur->symbol = s;
    cur = cur->next;
  }
}

void symSTMT(STMT *obj, SymbolTable *sym)
{ SymbolTable *localsym;

  if (!obj) return;
  if(obj->next) { symSTMT(obj->next, sym); }

  switch(obj->kind) {
    case emptyK:
      return;
    case blockK:
      localsym = scopeSymbolTable(sym);
      symSTMT(obj->val.blockS, localsym);
      break;
    case varK:
      symVAR_DECL(obj->val.varS, sym);
      break;
    case typeK:
      symTYPE_DECL(obj->val.typeS, sym);
      break;
    case printK:
      symEXP(obj->val.printS, sym);
      break;
    case printlnK:
      symEXP(obj->val.printlnS, sym);
      break;
    case shortvarK:
      symSHORTVAR(obj->val.shortvarS.left, sym);// process left  //tODO
        symEXP(obj->val.shortvarS.right, sym); // process right
      break;
    case returnK:
      symEXP(obj->val.returnS, sym);
      break;
    case ifK:
      localsym = scopeSymbolTable(sym);
      if(obj->val.ifS.pre_stmt) {
        symSTMT(obj->val.ifS.pre_stmt, localsym);
      }
      symEXP(obj->val.ifS.condition, localsym);
      symSTMT(obj->val.ifS.body, localsym);
      break;
    case ifelseK:
      localsym = scopeSymbolTable(sym);
      if(obj->val.ifelseS.pre_stmt) {
        symSTMT(obj->val.ifelseS.pre_stmt, localsym);
      }
      symEXP(obj->val.ifelseS.condition, localsym);
      symSTMT(obj->val.ifelseS.thenpart, localsym);
      symSTMT(obj->val.ifelseS.elsepart, localsym);
      break;
    case switchK:
      localsym = scopeSymbolTable(sym);
      if(obj->val.switchS.pre_stmt) {
        symSTMT(obj->val.switchS.pre_stmt, localsym);

        if(obj->val.switchS.condition) {
          symEXP(obj->val.switchS.condition, localsym);
        }

        symCASE_DECL(obj->val.switchS.body, localsym);
        break;
        case forK:
        localsym = scopeSymbolTable(sym);
        symFOR_CLAUSE(obj->val.forS.for_clause, localsym);
        symSTMT(obj->val.forS.body, localsym);
        break;
        case breakK:
        case continueK:
        break;
        case assignK:
        symEXP(obj->val.assignS.left, sym);
        symEXP(obj->val.assignS.right, sym);
        break;
        case expK:
        symEXP(obj->val.expS, sym);
        break;
      }
  }
}

void symCASE_DECL(CASE_DECL *obj, SymbolTable *sym)
{ SymbolTable *localsym = scopeSymbolTable(sym);

  if (!obj) return;
  if(obj->next) symCASE_DECL(obj->next, sym);

  switch(obj->kind) {
    case caseK:
      symEXP(obj->val.caseC.condition, localsym);
      symSTMT(obj->val.caseC.stmt, localsym);
      break;
    case defaultK:
      symSTMT(obj->val.defaultC, localsym);
      break;
  }
}

void symFOR_CLAUSE(FOR_CLAUSE *obj, SymbolTable *sym)
{ if(obj->init_stmt) symSTMT(obj->init_stmt, sym);

  if(obj->condition) {
    symEXP(obj->condition, sym);
  }

  if(obj->post_stmt) {
    symSTMT(obj->post_stmt, sym);
  }
}

void symEXP(EXP *obj, SymbolTable *sym)
{ if (!obj) return;
  if(obj->next) { symEXP(obj->next, sym); }

  switch(obj->kind) {
    case idK:
      obj->val.idE.id->symbol = getSymbol(sym, obj->val.idE.id->name);
      break;
    case intconstK:
    case floatconstK:
    case runeconstK:
    case stringconstK:
    case rawstringconstK:
      break;
    case plusK:
    case minusK:
    case timesK:
    case divK:
    case modK:
    case bitandK:
    case andnotK:
    case bitorK:
    case bitxork:
    case leftshiftK:
    case rightshiftK:
    case eqK:
    case neqK:
    case leK:
    case leqK:
    case geK:
    case geqK:
    case orK:
    case andK:
      symEXP(obj->val.binaryE.left, sym);
      symEXP(obj->val.binaryE.right, sym);
      break;
    case unotK:
    case uplusK:
    case uminusK:
    case ubitnotK:
      symEXP(obj->val.unaryE, sym);
      break;
    case indexK:
      symEXP(obj->val.indexE.exp, sym);
      symEXP(obj->val.indexE.index, sym);
      break;
    case selectorK:
      symEXP(obj->val.selectorE.exp, sym);
      obj->val.idE.id->symbol = getSymbol(sym, obj->val.idE.id->name);
      break;
    case funccallK:
      symEXP(obj->val.funccallE.exp, sym);
      if (obj->val.funccallE.args) symEXP(obj->val.funccallE.args, sym);
      break;
    case appendK:
      obj->val.appendE.id->symbol = getSymbol(sym, obj->val.appendE.id->name);
      symEXP(obj->val.appendE.exp, sym);
      break;
    case castK:
      /* symTYPE(obj->val.castE.type, sym); */
      symEXP(obj->val.castE.exp, sym);
      break;
    case parenK:
      symEXP(obj->val.parenE, sym);
      break;
  }
}


void symSHORTVAR(EXP *obj, SymbolTable *sym) {
  //dan, please indulge me
}

/* Returns the SymbolKind associated with TYPE `type` */
SymbolKind symKindFromTYPE(TYPE *type)
{ SymbolKind kind;

  switch (type->kind) {
    case type_refK:
      /* type.val.refT.id->symbol = getSymbol(sym, type.val.refT.id->name, type->loc); */
      kind = typeSym;
      break;
    case type_intK:
      kind = intSym;
      break;
    case type_floatK:
      kind = floatSym;
      break;
    case type_boolK:
      kind = boolSym;
      break;
    case type_runeK:
      kind = runeSym;
      break;
    case type_stringK:
      kind = stringSym;;
      break;
    case type_arrayK:
      kind = arraySym;
      break;
    case type_sliceK:
      kind = sliceSym;
      break;
    case type_structK:
      kind = structSym;
      break;
  }

  return kind;
}

