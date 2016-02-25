#include "weed.h"

void weedPROGRAM(PROGRAM *program) {
	weedPACKAGE(program->package);
	weedTOP_DECL(program->top_decl);
}

void weedPACKAGE(PACKAGE *p) {
	weedIDblank(p->id);
}

void weedTOP_DECL(TOP_DECL *decl) {
	if (!decl) return;
	if (decl->next) weedTOP_DECL(decl->next);

	switch(decl->kind) {
		case top_varK:
			weedTOPvar(decl->val.varT);
			break;
		case top_typeK:
			weedTOPtype(decl->val.typeT);
			break;
		case top_funcK:
			weedTOPfunc(decl->val.funcT);
			break;
	}
}

void  weedTOPvar(VAR_DECL *decl) {
	if (!decl) return;
	if (decl->next) weedTOPvar(decl->next);

	weedTYPE(decl->type);
	weedVARdecl(decl->id, decl->exp, decl->loc);
}

/* #id = #exp */
void  weedVARdecl(ID *id, EXP *exp, YYLTYPE loc) {
	if (!exp) return;

	while (id && exp) {
		id = id->next;
		exp = exp->next;
	}

	if (!(id == NULL && exp == NULL))
		printError("Number of ids doesn't match number of expressions", loc);
}

void weedTOPtype(TYPE_DECL *decl) {
	if (!decl) return;
	if (decl->next) weedTOPtype(decl->next);

	weedTYPE(decl->type);
}

void weedTYPE(TYPE *type) {
	if (!type) return;

	switch (type->kind) {
		case type_refK:
			weedIDblank(type->val.refT.id);
			break;
		case type_structK:
			weedSTRUCTdecl(type->val.structT.struct_decl);
			break;
		case type_arrayK:
			weedTYPE(type->val.arrayT.type);
			break;
		case type_sliceK:
			weedTYPE(type->val.sliceT.type);
			break;
		default:
			return;
	}
}

void weedSTRUCTdecl(STRUCT_DECL *decl) {
	if (!decl) return;
	if (decl->next) weedSTRUCTdecl(decl->next);

	weedTYPE(decl->type);
}

void weedTOPfunc(FUNC_DECL *func) {
	if (!func) return;

	/* weedIDblank(func->id); */

	if (!weedSTMTfuncreturn(func->body, weedFUNC_SIGN(func->signature))) {
		printError("missing return at end of function", func->loc);
	}

	weedSTMT(func->body, false, false);
}

int weedFUNC_SIGN(FUNC_SIGN *signature) {
	if (!signature) return false;

	weedFUNC_ARG(signature->arg);

	if (signature->type) {
		weedTYPE(signature->type);
		return true;
	} else {
		return false;
	}
}

void weedFUNC_ARG(FUNC_ARG *arg) {
	if (!arg) return;
	if (arg->next) weedFUNC_ARG(arg->next);

	weedTYPE(arg->type);
}

void weedSTMT(STMT *stmt, int isInsideLoop, int isInsideSwitch) {
	if (!stmt) return;
	if (stmt->next) weedSTMT(stmt->next, isInsideLoop, isInsideSwitch);

	switch(stmt->kind) {
		case emptyK:
			break;
		case blockK:
			weedSTMT(stmt->val.blockS, isInsideLoop, isInsideSwitch);
			break;
		case printK:
			weedEXP(stmt->val.printS);
			break;
		case printlnK:
			weedEXP(stmt->val.printlnS);
			break;
		case returnK:
			weedEXP(stmt->val.returnS);
			break;
		case varK:
			weedTOPvar(stmt->val.varS);
			break;
		case typeK:
			weedTOPtype(stmt->val.typeS);
			break;
		case expK:
			weedSTMTexp(stmt->val.expS);
			weedEXP(stmt->val.expS);
			break;
		case shortvarK:
			weedSTMTshorvar(stmt->val.shortvarS.left, stmt->val.shortvarS.right);
			weedEXP(stmt->val.shortvarS.right);
			break;
		case assignK:
			weedSTMTassign(stmt->val.assignS.left, stmt->val.assignS.right);
			weedEXPlvalue(stmt->val.assignS.left);
			weedEXP(stmt->val.assignS.right);
			break;
		case ifK:
			weedSTMT(stmt->val.ifS.pre_stmt, false, false);
			weedEXP(stmt->val.ifS.condition);
			weedSTMT(stmt->val.ifS.body, isInsideLoop, isInsideSwitch);
			break;
		case ifelseK:
			weedSTMT(stmt->val.ifelseS.pre_stmt, false, false);
			weedEXP(stmt->val.ifelseS.condition);
			weedSTMT(stmt->val.ifelseS.thenpart, isInsideLoop, isInsideSwitch);
			weedSTMT(stmt->val.ifelseS.elsepart, isInsideLoop, isInsideSwitch);
			break;
		case forK:
			weedFOR_CLAUSE(stmt->val.forS.for_clause);
			weedSTMT(stmt->val.forS.body, true, isInsideSwitch);
			break;
		case switchK:
			weedSTMT(stmt->val.switchS.pre_stmt, false, false);
			weedEXP(stmt->val.switchS.condition);
			weedSTMTswitch(stmt->val.switchS.body, isInsideLoop);
			break;
		case breakK:
			if (!isInsideLoop && !isInsideSwitch) printError("break is not in a switch/loop", stmt->loc);
			break;
		case continueK:
			if (!isInsideLoop) printError("continue is not in a loop", stmt->loc);
			break;
	}
}

void weedFOR_CLAUSE(FOR_CLAUSE *clause) {
	weedSTMT(clause->init_stmt, false, false);

	weedEXP(clause->condition);

	if (clause->post_stmt) {
		if (clause->post_stmt->kind == shortvarK) {
			printError("cannot declare in the for-increment", clause->post_stmt->loc);
		}
		weedSTMT(clause->post_stmt, false, false);
	}
}

int weedSTMTfuncreturn(STMT *stmt, int isValuedReturn) {
	if (!stmt) return !isValuedReturn;
	while (stmt->kind == emptyK) {
		if (stmt->next) {
			stmt = stmt->next;
		} else {
			break;
		}
	}

	switch(stmt->kind){
		case returnK:
			if(!isValuedReturn && stmt->val.returnS) {
				printError("too many return values", stmt->loc);
			} else if (isValuedReturn && !stmt->val.returnS) {
				printError("not enough arguments to return", stmt->loc);
			} else {
				return true;
			}
		case ifelseK:
			if (weedSTMTfuncreturnifelse(stmt, isValuedReturn)) return true;
			break;
		case forK:
			if (weedSTMTfuncreturnfor(stmt, isValuedReturn)) return true;
			break;
		case switchK:
			if (weedSTMTfuncreturnswitch(stmt->val.switchS.body, isValuedReturn)) return true;
			break;
		default:
			break;
	}

	return !isValuedReturn;
}

int weedSTMTfuncreturnifelse(STMT *stmt, int isValuedReturn) {
	if (!stmt) return false;

	if (weedSTMTfuncreturn(stmt->val.ifelseS.thenpart, isValuedReturn) &&
				weedSTMTfuncreturn(stmt->val.ifelseS.elsepart, isValuedReturn)) {
		return true;
	} else {
		return false;
	}
}

int weedSTMTfuncreturnfor(STMT *stmt, int isValuedReturn) {
	if (!stmt->val.forS.for_clause->condition) {

		if (weedSTMTfuncreturn(stmt->val.forS.body, isValuedReturn)) {
			return !findSTMTbreak(stmt->val.forS.body);
		} else {
			return isValuedReturn;
		}

	} else {
		return false;
	}
}

int weedSTMTfuncreturnswitch(CASE_DECL *c, int isValuedReturn) {
	int foundDefaultCase;
	if (!c) return isValuedReturn;

	foundDefaultCase = false;

	for ( ; c; c = c->next) {
		switch (c->kind) {
			case caseK:
				if (findSTMTbreak(c->val.caseC.stmt)) return false;
				if (!weedSTMTfuncreturn(c->val.caseC.stmt, isValuedReturn)) return false;
				break;
			case defaultK:
				foundDefaultCase = true;
				if (findSTMTbreak(c->val.defaultC)) return false;
				if (!weedSTMTfuncreturn(c->val.defaultC, isValuedReturn)) return false;
				break;
		}
	}

	return foundDefaultCase;
}

int findSTMTbreak(STMT *stmt) {
	if (!stmt) return false;
	if (stmt->next) {
		if (findSTMTbreak(stmt->next)) return true;
	}

	switch(stmt->kind) {
		default:
			return false;
			break;
		case blockK:
			return findSTMTbreak(stmt->val.blockS);
			break;
		case ifK:
			return findSTMTbreak(stmt->val.ifS.body);
			break;
		case ifelseK:
			return findSTMTbreak(stmt->val.ifelseS.thenpart) ||
					findSTMTbreak(stmt->val.ifelseS.elsepart);
			break;
		case breakK:
			return true;
			break;
	}
}

/* only 1 default case */
void weedSTMTswitch(CASE_DECL *body, int isInsideLoop) {
	YYLTYPE loc;
	int numDefault;

	if (!body) return;

	/* count number of default cases and traverse */
	numDefault = 0;
	while (body) {
		switch (body->kind) {
			case caseK:
				weedEXP(body->val.caseC.condition);
				weedSTMT(body->val.caseC.stmt, isInsideLoop, true);
				break;
			case defaultK:
				weedSTMT(body->val.defaultC, isInsideLoop, true);
				numDefault += 1;
				break;
		}

		loc = body->loc;
		body = body->next;
	}

	if (numDefault > 1) printError("Too many default cases", loc);
}

void weedSTMTshorvar(EXP *left, EXP *right) {
	/* only allow id on left side of := */
	if (left->kind != idK) printError("non-name on left side of :=", left->loc);

	weedSTMTassign(left, right);
}

void weedSTMTassign(EXP *left, EXP *right) {
	YYLTYPE loc = right->loc;

	/* count mismatch */
	while (left && right) {
		left = left->next;
		right = right->next;
	}

	if (left || right)
		printError("Number of ids doesn't match number of expressions", loc);
}

void weedSTMTexp(EXP *exp) {
	while (exp->kind == parenK) exp = exp->val.parenE;

	if (exp->kind != funccallK) {
		printError("expression is not a call", exp->loc);
	}
}

void weedEXP(EXP *exp) {
	if (!exp) return;
	if (exp->next) weedEXP(exp->next);

	switch (exp->kind) {
		/* Constants */
		case idK:
			weedIDblank(exp->val.idE.id);
			break;
		case intconstK:
		case floatconstK:
		case runeconstK:
		case stringconstK:
		case rawstringconstK:
			break;

		/* Weed division by zero */
		case divK:
		case modK:
			weedEXPdivzero(exp->val.binaryE.right);

		/* Binary operators */
		case plusK:
		case minusK:
		case timesK:
		case bitandK:
		case andnotK:
		case bitorK:
		case bitxork:
		case leftshiftK:
		case rightshiftK:
		case neqK:
		case eqK:
		case leK:
		case leqK:
		case geK:
		case geqK:
		case orK:
		case andK:
			weedEXP(exp->val.binaryE.left);
			weedEXP(exp->val.binaryE.right);
			break;

		 /* Unary operators */
		case uplusK:
		case uminusK:
		case unotK:
		case ubitnotK:
			weedEXP(exp->val.unaryE);
			break;

		case selectorK:
			weedEXP(exp->val.selectorE.exp);
			weedEXPlvalue(exp->val.selectorE.exp);
			weedIDblank(exp->val.selectorE.id);
			break;
		case indexK:
			weedEXP(exp->val.indexE.exp);
			weedEXPlvalue(exp->val.indexE.exp);
			break;
		case funccallK:
			weedEXPcallId(exp->val.funccallE.exp);
			weedEXP(exp->val.funccallE.args);
			break;
		case parenK:
			weedEXP(exp->val.parenE);
			break;
		case appendK:
			weedIDblank(exp->val.appendE.id);
			weedEXP(exp->val.appendE.exp);
			break;
		case castK:
			weedEXP(exp->val.castE.exp);
			break;
	}
}

void weedEXPdivzero(EXP *exp) {
	switch (exp->kind) {
		case intconstK:
			if (exp->val.intconstE == 0) printError("division by 0", exp->loc);
			break;
		case (floatconstK):
			if (exp->val.floatconstE == 0.0) printError("division by 0.0", exp->loc);
			break;
		default:
			return;
	}
}

void weedEXPcallId(EXP *exp) {
	while (exp->kind == parenK) exp = exp->val.parenE;

	if (exp->kind != idK) {
		printErrorMsg("functions call expects and IDENTIFIER");
	} else {
		weedIDblank(exp->val.idE.id);
	}
}

void weedEXPlvalue(EXP *exp) {
	if (exp->next) weedEXPlvalue(exp->next);
	while (exp->kind == parenK) exp = exp->val.parenE;

	if (exp->kind != idK && exp->kind != selectorK && exp->kind != indexK)
		printError("expression is not assignable", exp->loc);
}

void weedIDblank(ID *id) {
	if (strcmp(id->name, "_") == 0) {
		printError("cannot _ use as value", id->loc);
	}
}
