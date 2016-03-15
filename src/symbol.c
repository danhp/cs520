#include "symbol.h"

void symPROGRAM(PROGRAM *obj, int print) {
  shouldPrint = print;
  SymbolTable *sym = initSymbolTable();

  // add true/false
  putSymbol(sym, "true", boolSym, yylloc);
  putSymbol(sym, "false", boolSym, yylloc);

  sym = scopeSymbolTable(sym);
  symTOP_DECL(obj->top_decl, sym);
  sym = unscopeSymbolTable(sym, obj->loc.first_line);
}

void symTOP_DECL(TOP_DECL *obj, SymbolTable *sym) {
  if (obj->next) symTOP_DECL(obj->next, sym);

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

void symVAR_DECL(VAR_DECL *obj, SymbolTable *sym) {
	SymbolKind kind;

	if(!obj) return;
	if(obj->next) symVAR_DECL(obj->next, sym);

	if (obj->type) {
		symTYPE(obj->type, sym);
		kind = symKindFromTYPE(obj->type);
	} else {
		kind = inferredSym;
	}

	ID *cur = obj->id;
	while (cur) {
		SYMBOL *s = putSymbol(sym, cur->name, kind, obj->loc);
		if (kind == structSym) {
			s->val.structDecl = obj->type->val.structT;
		}
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
      obj->val.refT->symbol = getSymbol(sym, obj->val.refT->name, obj->loc);;
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
      if(obj->val.sliceT) {
        symTYPE(obj->val.sliceT, sym);
      }
      break;
    case type_structK:
      if(obj->val.structT) {
        sym = scopeSymbolTable(sym);
        symSTRUCT_DECL(obj->val.structT, sym);
        sym = unscopeSymbolTable(sym, obj->loc.first_line);
      }
      return;
  }
}

void symSTRUCT_DECL(STRUCT_DECL *obj, SymbolTable *sym) {
	if (!obj) return;
	if (obj->next) { symSTRUCT_DECL(obj->next, sym); }

	ID *id = obj->id;

	symTYPE(obj->type, sym);
	while (id) {
		id->symbol = putSymbol(sym, id->name, symKindFromTYPE(obj->type), obj->loc);
		if (id->symbol->kind == structSym) {
			id->symbol->val.structDecl = obj->type->val.structT;
		}
		id = id->next;
	}
}

/* 1)Add function type to top-level symbol table
 * 2)Create local symbol table and add parameters to it
 */
void symFUNC_DECL(FUNC_DECL *obj, SymbolTable *sym)
{ SYMBOL *s;

  s = putSymbol(sym, obj->id->name, funcSym, obj->loc);
  s->val.funcSignature = obj->signature;
  obj->id->symbol = s;

  sym = scopeSymbolTable(sym);
  symFUNC_SIGN(obj->signature, sym);
  symSTMT(obj->body, sym);
  unscopeSymbolTable(sym, obj->loc.first_line);
}

void symFUNC_SIGN(FUNC_SIGN *obj, SymbolTable *sym) {
	if (obj->arg) {
		symFUNC_ARG(obj->arg, sym);
	}

	if (obj->type) {
		symTYPE(obj->type, sym);
	}
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

void symSTMT(STMT *obj, SymbolTable *sym) {

  if (!obj) return;
  if(obj->next) { symSTMT(obj->next, sym); }

  switch(obj->kind) {
    case emptyK:
      return;
    case blockK:
      sym = scopeSymbolTable(sym);
      symSTMT(obj->val.blockS, sym);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
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
      sym = scopeSymbolTable(sym);
      if(obj->val.ifS.pre_stmt) {
        symSTMT(obj->val.ifS.pre_stmt, sym);
      }
      symEXP(obj->val.ifS.condition, sym);
      symSTMT(obj->val.ifS.body, sym);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case ifelseK:
      sym = scopeSymbolTable(sym);
      if(obj->val.ifelseS.pre_stmt) {
        symSTMT(obj->val.ifelseS.pre_stmt, sym);
      }
      symEXP(obj->val.ifelseS.condition, sym);
      symSTMT(obj->val.ifelseS.thenpart, sym);
      symSTMT(obj->val.ifelseS.elsepart, sym);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case switchK:
      sym = scopeSymbolTable(sym);
      if(obj->val.switchS.pre_stmt) {
          symSTMT(obj->val.switchS.pre_stmt, sym);
      }

      if(obj->val.switchS.condition) {
          symEXP(obj->val.switchS.condition, sym);
      }

      symCASE_DECL(obj->val.switchS.body, sym);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case forK:
      sym = scopeSymbolTable(sym);
      symFOR_CLAUSE(obj->val.forS.for_clause, sym);
      symSTMT(obj->val.forS.body, sym);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
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

void symCASE_DECL(CASE_DECL *obj, SymbolTable *sym) {
  if (!obj) return;
  if(obj->next) symCASE_DECL(obj->next, sym);

  sym = scopeSymbolTable(sym);
  switch(obj->kind) {
    case caseK:
      symEXP(obj->val.caseC.condition, sym);
      symSTMT(obj->val.caseC.stmt, sym);
      break;
    case defaultK:
      symSTMT(obj->val.defaultC, sym);
      break;
  }
  sym = unscopeSymbolTable(sym, obj->loc.first_line);
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
      obj->val.idE->symbol = getSymbol(sym, obj->val.idE->name, obj->loc);
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
      /* Defer the field checking to later */
      /* obj->val.selectorE.id->symbol = getSymbol(sym, obj->val.selectorE.id->name, obj->loc); */
      break;
    case funccallK:
      symEXP(obj->val.funccallE.exp, sym);
      if (obj->val.funccallE.args) symEXP(obj->val.funccallE.args, sym);
      break;
    case appendK:
      obj->val.appendE.id->symbol = getSymbol(sym, obj->val.appendE.id->name, obj->loc);
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
	if (!obj) return;
	if (obj->next) symSHORTVAR(obj->next, sym);

	obj->val.idE->symbol = putSymbol(sym, obj->val.idE->name, inferredSym, obj->loc);
}

/* Returns the SymbolKind associated with TYPE `type` */
SymbolKind symKindFromTYPE(TYPE *type)
{ SymbolKind kind;

  switch (type->kind) {
    case type_refK:
      kind = symKindFromTYPE(type->val.refT->symbol->val.type);
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

