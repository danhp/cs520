#include "symbol.h"

void symPROGRAM(PROGRAM *obj, int print) {
	shouldPrint = print;
	sym = initSymbolTable();

	// add true/false
	TYPE *boolType = NEW(TYPE);
	boolType->kind = type_boolK;
	putSymbol(sym, "true", boolType, yylloc);
	putSymbol(sym, "false", boolType, yylloc);

	sym = scopeSymbolTable(sym);
	symTOP_DECL(obj->top_decl);
	sym = unscopeSymbolTable(sym, obj->loc.first_line);
}

void symTOP_DECL(TOP_DECL *obj) {
	if (obj->next) symTOP_DECL(obj->next);

	switch(obj->kind) {
		case top_varK:
			symVAR_DECL(obj->val.varT);
			break;
		case top_typeK:
			symTYPE_DECL(obj->val.typeT);
			break;
		case top_funcK:
			symFUNC_DECL(obj->val.funcT);
			break;
	}
}

void symVAR_DECL(VAR_DECL *obj) {
	if(!obj || !obj->id) return;
	if(obj->next) symVAR_DECL(obj->next);

	ID *cur = obj->id;
	while (cur) {
		if (!obj->type) {
			cur->symbol = putSymbol(sym, cur->name, obj->type, cur->loc);
			cur->symbol->kind = inferredSym;
		} else {
			symTYPE(obj->type);
			cur->symbol = putSymbol(sym, cur->name, obj->type, cur->loc);
		}
		cur = cur->next;
	}

	if (obj->exp) {
		symEXP(obj->exp);
	}
}

void symTYPE_DECL(TYPE_DECL *obj) {
	if(!obj) return;
	if(obj->next) symTYPE_DECL(obj->next);

	symTYPE(obj->type);
	obj->id->symbol = putSymbol(sym, obj->id->name, obj->type, obj->loc);
	obj->id->symbol->kind = typeSym;
}

void symTYPE(TYPE *obj) {
	if(!obj) return;

	switch(obj->kind) {
		case type_refK:
			obj->val.refT->symbol = getSymbol(sym, obj->val.refT->name, obj->loc);
			if (obj->val.refT->symbol->kind != typeSym) {
				printErrorMsg("Refered value is not a type");
			}
			return;
		case type_intK:
		case type_floatK:
		case type_boolK:
		case type_runeK:
		case type_stringK:
			break;
		case type_arrayK:
			symTYPE(obj->val.arrayT.type);
			break;
		case type_sliceK:
			symTYPE(obj->val.sliceT);
			break;
		case type_structK:
			sym = scopeSymbolTable(sym);
			symSTRUCT_DECL(obj->val.structT);
			sym = unscopeSymbolTable(sym, obj->loc.first_line);
			return;
	}
}

void symSTRUCT_DECL(STRUCT_DECL *obj) {
	if (!obj) return;
	if (obj->next) symSTRUCT_DECL(obj->next);

	ID *id = obj->id;

	symTYPE(obj->type);
	while (id) {
		id->symbol = putSymbol(sym, id->name, obj->type, obj->loc);
		id = id->next;
	}
}

/* 1)Add function type to top-level symbol table
 * 2)Create local symbol table and add parameters to it
 */
void symFUNC_DECL(FUNC_DECL *obj) {
	if (!obj) return;
	SYMBOL *s;

	// HACK: fixup the symbol after passing null
	s = putSymbol(sym, obj->id->name, NULL, obj->loc);
	s->kind = funcSym;
	s->val.funcSignature = obj->signature;
	obj->id->symbol = s;

	sym = scopeSymbolTable(sym);
	symFUNC_SIGN(obj->signature);
	symSTMT(obj->body);
	sym = unscopeSymbolTable(sym, obj->loc.first_line);
}

void symFUNC_SIGN(FUNC_SIGN *obj) {
	if (obj->arg) {
		symFUNC_ARG(obj->arg);
	}

	if (obj->type) {
		symTYPE(obj->type);
	}
}

void symFUNC_ARG(FUNC_ARG *obj)
{ if (!obj) return;
  if(obj->next) { symFUNC_ARG(obj->next); }

  symTYPE(obj->type);
  ID *cur = obj->id;
  while (cur) {
    SYMBOL *s = putSymbol(sym, cur->name, obj->type, obj->loc);
    cur->symbol = s;
    cur = cur->next;
  }
}

void symSTMT(STMT *obj) {
  if (!obj) return;
  if(obj->next) { symSTMT(obj->next); }

  switch(obj->kind) {
    case emptyK:
      return;
    case blockK:
      sym = scopeSymbolTable(sym);
      symSTMT(obj->val.blockS);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case varK:
      symVAR_DECL(obj->val.varS);
      break;
    case typeK:
      symTYPE_DECL(obj->val.typeS);
      break;
    case printK:
      symEXP(obj->val.printS);
      break;
    case printlnK:
      symEXP(obj->val.printlnS);
      break;
    case shortvarK:
      symSHORTVAR(obj->val.shortvarS.left);// process left
      symEXP(obj->val.shortvarS.right); // process right
      break;
    case returnK:
      symEXP(obj->val.returnS);
      break;
    case ifK:
      sym = scopeSymbolTable(sym);
      if(obj->val.ifS.pre_stmt) {
        symSTMT(obj->val.ifS.pre_stmt);
      }
      symEXP(obj->val.ifS.condition);
      symSTMT(obj->val.ifS.body);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case ifelseK:
      sym = scopeSymbolTable(sym);
      if(obj->val.ifelseS.pre_stmt) {
        symSTMT(obj->val.ifelseS.pre_stmt);
      }
      symEXP(obj->val.ifelseS.condition);
      symSTMT(obj->val.ifelseS.thenpart);
      symSTMT(obj->val.ifelseS.elsepart);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case switchK:
      sym = scopeSymbolTable(sym);
      if(obj->val.switchS.pre_stmt) {
          symSTMT(obj->val.switchS.pre_stmt);
      }

      if(obj->val.switchS.condition) {
          symEXP(obj->val.switchS.condition);
      }

      symCASE_DECL(obj->val.switchS.body);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case forK:
      sym = scopeSymbolTable(sym);
      symFOR_CLAUSE(obj->val.forS.for_clause);
      symSTMT(obj->val.forS.body);
      sym = unscopeSymbolTable(sym, obj->loc.first_line);
      break;
    case breakK:
    case continueK:
      break;
    case assignK:
      symEXP(obj->val.assignS.left);
      symEXP(obj->val.assignS.right);
      break;
    case expK:
      symEXP(obj->val.expS);
      break;
    }
}

void symCASE_DECL(CASE_DECL *obj) {
  if (!obj) return;
  if(obj->next) symCASE_DECL(obj->next);

  sym = scopeSymbolTable(sym);
  switch(obj->kind) {
    case caseK:
      symEXP(obj->val.caseC.condition);
      symSTMT(obj->val.caseC.stmt);
      break;
    case defaultK:
      symSTMT(obj->val.defaultC);
      break;
  }
  sym = unscopeSymbolTable(sym, obj->loc.first_line);
}

void symFOR_CLAUSE(FOR_CLAUSE *obj)
{ if(obj->init_stmt) symSTMT(obj->init_stmt);

  if(obj->condition) {
    symEXP(obj->condition);
  }

  if(obj->post_stmt) {
    symSTMT(obj->post_stmt);
  }
}

void symEXP(EXP *obj)
{ if (!obj) return;
  if(obj->next) { symEXP(obj->next); }

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
      symEXP(obj->val.binaryE.left);
      symEXP(obj->val.binaryE.right);
      break;
    case unotK:
    case uplusK:
    case uminusK:
    case ubitnotK:
      symEXP(obj->val.unaryE);
      break;
    case indexK:
      symEXP(obj->val.indexE.exp);
      symEXP(obj->val.indexE.index);
      break;
    case selectorK:
      symEXP(obj->val.selectorE.exp);
      break;
    case funccallK:
      symEXP(obj->val.funccallE.exp);
      if (obj->val.funccallE.args) symEXP(obj->val.funccallE.args);
      break;
    case appendK:
      obj->val.appendE.id->symbol = getSymbol(sym, obj->val.appendE.id->name, obj->loc);
      TYPE *t = obj->val.appendE.id->symbol->val.type;
      obj->val.appendE.id->symbol->val.type = t;
      symEXP(obj->val.appendE.exp);
      break;
    case castK:
      symTYPE(obj->val.castE.type);
      symEXP(obj->val.castE.exp);
      break;
    case parenK:
      symEXP(obj->val.parenE);
      break;
  }
}


void symSHORTVAR(EXP *id) {
	if (!id) return;

	YYLTYPE loc = id->loc;
	int foundNewDeclaration = 0;
	while (id) {
			if (isInTopFrame(sym, id->val.idE->name)) {
				if (strcmp(id->val.idE->name, "_")) {
					id->val.idE->symbol = blank();
				} else {
					id->val.idE->symbol = getSymbol(sym, id->val.idE->name, id->loc);
				}
			} else {
				foundNewDeclaration = 1;
				id->val.idE->symbol = putSymbol(sym, id->val.idE->name, NULL, id->loc);
				id->val.idE->symbol->kind = inferredSym;
			}
		id = id->next;
	}

	if (!foundNewDeclaration) {
		printError("no new delcaration on left of :=", loc);
	}
}

