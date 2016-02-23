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
	weedFUNC_SIGN(func->signature);
	weedSTMT(func->body);
}

void weedFUNC_SIGN(FUNC_SIGN *signature){ }

void weedSTMT(STMT *stmt) {
	if (stmt->next) weedSTMT(stmt->next);

	switch(stmt->kind) {
		case emptyK:
			break;
		case varK:
			weedVARdecl(stmt->val.varS->id, stmt->val.varS->exp, stmt->val.varS->loc);
			break;
		case typeK:
			break;
		case switchK:
			weedSTMTswitch(stmt->val.switchS.body);
			break;
		case expK:
			weedEXP(stmt->val.expS);
			break;
		case shortvarK:
			break;
		case assignK:
			break;
		case printK:
		case printlnK:
		case returnK:
			break;
		case ifK:
		case ifelseK:
			break;
		case forK:
			break;
		case breakK:
		case continueK:
			break;
	}
}

/* only 1 default case */
void weedSTMTswitch(CASE_DECL *body) {
	YYLTYPE loc;

	if (!body) return;

	/* count number of default cases */
	int numDefault = 0;
	while (body) {
		if (body->kind == defaultK) numDefault += 1;
		loc = body->loc;
		body = body->next;
	}

	if (numDefault > 1) printError("Too many default cases", loc);
}

void weedEXP(EXP *exp) {
	if (exp->next) weedEXP(exp->next);

	switch (exp->kind) {
		// Constants
		case idK:
		case intconstK:
		case floatconstK:
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

		case selectorK:
		case indexK:
		case funccallK:
		case appendK:
		case castK:
			break;
	}
}

void weedEXPdivzero(EXP *exp) {
	switch (exp->kind) {
		case intconstK:
			if (exp->val.intconstE == 0) printError("division by 0", exp->loc);
			break;
		case (floatconstK):
			if (exp->val.floatconstE == 0.0) printError("division by 0", exp->loc);
			break;
		default:
			break;
	}
}
