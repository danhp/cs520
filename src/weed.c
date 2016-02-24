#include "weed.h"

void weedPROGRAM(PROGRAM *program) {
	if (program->top_decl != NULL)
		weedTOP_DECL(program->top_decl);
}

void weedTOP_DECL(TOP_DECL *decl) {
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
	if (decl) {
		if (decl->next) weedTOPvar(decl->next);
		if (decl->exp) weedVARdecl(decl->id, decl->exp, decl->loc);
	}
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

void weedTOPtype(TYPE_DECL *decl) { }

void weedTOPfunc(FUNC_DECL *func) {
	if (!weedSTMTfuncreturn(func->body, weedFUNC_SIGN(func->signature))) {
		printError("missing return at end of function", func->loc);
	}
	weedSTMT(func->body, false, false);
}

int weedFUNC_SIGN(FUNC_SIGN *signature){
	if (signature->type) {
		return true;
	} else {
		return false;
	}
}

void weedSTMT(STMT *stmt, int isInsideLoop, int isInsideSwitch) {
	if (!stmt) return;
	if (stmt->next) weedSTMT(stmt->next, isInsideLoop, isInsideSwitch);

	switch(stmt->kind) {
		case emptyK:
		case printK:
		case printlnK:
		case returnK:
			break;
		case varK:
			weedVARdecl(stmt->val.varS->id, stmt->val.varS->exp, stmt->val.varS->loc);
			break;
		case typeK:
			/* nothing to check */
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
			weedSTMTswitch(stmt->val.switchS.body, isInsideLoop);
			break;
		case breakK:
			if (!isInsideLoop && !isInsideSwitch) printError("break is not in a switch/loop", stmt->loc);
			break;
		case continueK:
			if (!isInsideLoop) printErrorMsg("continue is not in a loop");
			break;
	}
}

void weedFOR_CLAUSE(FOR_CLAUSE *clause) {
	weedSTMT(clause->init_stmt, false, false);

	weedEXP(clause->condition);

	if (clause->post_stmt) {
		if (clause->post_stmt->kind == shortvarK) printErrorMsg("cannot declare in the for-increment");
		weedSTMT(clause->post_stmt, false, false);
	}
}

int weedSTMTfuncreturn(STMT *stmt, int isValuedReturn) {
	if (stmt->next) {
		if (weedSTMTfuncreturn(stmt->next, isValuedReturn)) return true;
	}

	switch(stmt->kind){
		case returnK:
			if(!isValuedReturn && stmt->val.returnS) {
				printErrorMsg("too many return values");
			} else if (isValuedReturn && !stmt->val.returnS) {
				printErrorMsg("not enough arguments to return");
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


	if (isValuedReturn) {
		return false;
	} else {
		return true;
	}
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
		return weedSTMTfuncreturn(stmt->val.forS.body, isValuedReturn);
	} else {
		return false;
	}
}

int weedSTMTfuncreturnswitch(CASE_DECL *c, int isValuedReturn) {
	if (!c) return false;
	if (c->next) return weedSTMTfuncreturnswitch(c->next, isValuedReturn);

	if (c->kind == defaultK) {
		return weedSTMTfuncreturn(c->val.defaultC, isValuedReturn);
	} else {
		return false;
	}
}

/* only 1 default case */
void weedSTMTswitch(CASE_DECL *body, int isInsideLoop) {
	YYLTYPE loc;

	if (!body) return;

	/* count number of default cases and traverse */
	int numDefault = 0;
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
	if (left->kind != idK) printErrorMsg("non-name on left side of :=");

	weedSTMTassign(left, right);
}

void weedSTMTassign(EXP *left, EXP *right) {
	/* count mismatch */
	while (left && right) {
		left = left->next;
		right = right->next;
	}

	if (!(left == NULL && right == NULL))
		printErrorMsg("Number of ids doesn't match number of expressions");
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
		// Constants
		case idK:
		case intconstK:
		case floatconstK:
		case runeconstK:
		case stringconstK:
		case rawstringconstK:
			break;

		// Weed division by zero
		case divK:
			weedEXPdivzero(exp->val.binaryE.right);

		// Binary operators
		case plusK:
		case minusK:
		case timesK:
		case modK:
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

		// Unary operators
		case uplusK:
		case uminusK:
		case unotK:
		case ubitnotK:
			weedEXP(exp->val.unaryE);
			break;

		case selectorK:
			weedEXPlvalue(exp->val.selectorE.exp);
			break;
		case indexK:
			weedEXPlvalue(exp->val.indexE.exp);
			break;
		case funccallK:
			weedEXPfunccall(exp->val.funccallE.exp, exp->val.funccallE.args);
		case parenK:
			weedEXP(exp->val.parenE);
			break;
		case appendK:
		case castK:
			/* Do nothing */
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

void weedEXPfunccall(EXP *exp, EXP *args) {
	while (exp->kind == parenK) exp = exp->val.parenE;

	if (exp->kind != idK) {
		printErrorMsg("functions call expects and IDENTIFIER");
	}
}

void weedEXPlvalue(EXP *exp) {
	if (exp->next) weedEXPlvalue(exp->next);
	while (exp->kind == parenK) exp = exp->val.parenE;

	if (exp->kind != idK && exp->kind != selectorK && exp->kind != indexK)
		printErrorMsg("expression is not assignable");
}

void weedEXPid(EXP *exp) {
}
