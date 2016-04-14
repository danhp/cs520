#include "type.h"

TYPE *intTYPE, *floatTYPE, *stringTYPE, *runeTYPE, *boolTYPE;

void initTypes() {
	intTYPE = NEW(TYPE);
	intTYPE->kind = type_intK;

	floatTYPE = NEW(TYPE);
	floatTYPE->kind = type_floatK;

	stringTYPE = NEW(TYPE);
	stringTYPE->kind = type_stringK;

	runeTYPE = NEW(TYPE);
	runeTYPE->kind = type_runeK;

	boolTYPE = NEW(TYPE);
	boolTYPE->kind = type_boolK;
}

TYPE *getDeepestType(TYPE *type) {
	TYPE *t = type;
	while (t->kind == type_refK) {
		t = t->val.refT->symbol->val.type;
	}

	return t;
}

void typePROGRAM(PROGRAM *p) {
	initTypes();

	typePACKAGE(p->package);
	typeTOP_DECL(p->top_decl);
}

void typePACKAGE(PACKAGE *p) {
	if (strcmp(p->id->name, "main")) {
		printError("package name must be main", p->loc);
	}
}

void typeTOP_DECL(TOP_DECL *decl) {
	if (!decl) return;
	if (decl->next) typeTOP_DECL(decl->next);

	switch (decl->kind) {
		case top_varK:
			typeVAR_DECL(decl->val.varT);
			break;
		case top_typeK:
			typeTYPE_DECL(decl->val.typeT);
			break;
		case top_funcK:
			typeFUNC_DECL(decl->val.funcT);
			break;
	}
}

void typeVAR_DECL(VAR_DECL *decl) {
	if (!decl) return;
	if (decl->next) typeVAR_DECL(decl->next);

	ID *id = decl->id;
	EXP *exp = decl->exp;
	while (id && exp) {
		typeEXP(exp);

		if (id->symbol->kind != varSym &&
				id->symbol->kind != inferredSym &&
				id->symbol->kind != blankSym &&
				!exp->type) {
			printErrorAssign(id->name, id->loc);
		}

		/* Type inference */
		if (decl->type) {
			id->symbol->val.type = decl->type;
		} else {
			id->symbol->val.type = exp->type;
			decl->type = exp->type;
		}

		checkAssignable(decl->type, exp->type, decl->loc);

		id = id->next;
		exp = exp->next;
	}
}

void typeTYPE_DECL(TYPE_DECL *decl) {
	/* Nothing to do */
}

void typeFUNC_DECL(FUNC_DECL *decl) {
	if (!decl) return;

	if (strcmp(decl->id->name, "main") == 0) {
		if (decl->signature->arg || decl->signature->type) {
			printError("func main must have no arguments and no return values", decl->loc);
		}
	}

	typeSTMT(decl->body);

	/* Run after having annotated the tree */
	typeReturnsSTMT(decl->body, decl->signature->type);
}

void typeReturnsSTMT(STMT *stmt, TYPE *type) {
	if (!stmt || !type) return;
	if (stmt->next) typeReturnsSTMT(stmt->next, type);

	switch (stmt->kind) {
		case returnK:
			if (!isTypeAssignble(type, stmt->val.returnS->type)) {
				printErrorType(type, stmt->val.returnS->type, stmt->loc);
			}
			break;
		case ifK:
			typeReturnsSTMT(stmt->val.ifS.body, type);
			break;
		case ifelseK:
			typeReturnsSTMT(stmt->val.ifelseS.thenpart, type);
			typeReturnsSTMT(stmt->val.ifelseS.elsepart, type);
			break;
		case switchK:
			typeReturnsCASE(stmt->val.switchS.body, type);
			break;
		case forK:
			typeReturnsSTMT(stmt->val.forS.body, type);
			break;
		default:
			break;
	}
}

void typeReturnsCASE(CASE_DECL *decl, TYPE *type) {
	if (!decl) return;
	if (decl->next) typeReturnsCASE(decl->next, type);

	switch (decl->kind) {
		case caseK:
			typeReturnsSTMT(decl->val.caseC.stmt, type);
			break;
		case defaultK:
			typeReturnsSTMT(decl->val.defaultC, type);
			break;
	}
}

void typeSTRUCT_DECL(STRUCT_DECL *decl) {
	/* Nothing to do */
	printErrorMsg("dissalowed in line struct decl");
}

void typeSTMT(STMT *stmt) {
	if (!stmt) return;
	if (stmt->next) typeSTMT(stmt->next);

	TYPE *t;

	switch (stmt->kind) {
		case blockK:
			typeSTMT(stmt->val.blockS);
			break;
		case varK:
			typeVAR_DECL(stmt->val.varS);
			break;
		case typeK:
			typeTYPE_DECL(stmt->val.typeS);
			break;
		case shortvarK:
			typeSHORTVAR(stmt->val.shortvarS.left, stmt->val.shortvarS.right);
			break;
		case printK:
			typeEXP(stmt->val.printS);
			if (!isPrintableType(stmt->val.printS)) {
				printError("cannot print expression", stmt->loc);
			}
			break;
		case printlnK:
			typeEXP(stmt->val.printlnS);
			if (!isPrintableType(stmt->val.printlnS)) {
				printError("cannot print expression", stmt->loc);
			}
			break;
		case returnK:
			typeEXP(stmt->val.returnS);
			break;
		case ifK:
			typeSTMT(stmt->val.ifS.pre_stmt);
			t = typeEXP(stmt->val.ifS.condition);
			t = getDeepestType(t);
			if (t->kind != type_boolK) {
				printErrorMsg("type mismatch");
			}
			typeSTMT(stmt->val.ifS.body);
			break;
		case ifelseK:
			typeSTMT(stmt->val.ifelseS.pre_stmt);
			t = typeEXP(stmt->val.ifelseS.condition);
			t = getDeepestType(t);
			if (t->kind != type_boolK) {
				printErrorMsg("type mismatch");
			}
			typeSTMT(stmt->val.ifelseS.thenpart);
			typeSTMT(stmt->val.ifelseS.elsepart);
			break;
		case switchK:
			typeSTMT(stmt->val.switchS.pre_stmt);
			TYPE *t = typeEXP(stmt->val.switchS.condition);
			typeCASE_DECL(stmt->val.switchS.body, t);
			break;
		case forK:
			typeFOR_CLAUSE(stmt->val.forS.for_clause);
			typeSTMT(stmt->val.forS.body);
			break;
		case assignK:
			typeAssign(stmt->val.assignS.left, stmt->val.assignS.right);
			break;
		case expK:
			typeEXP(stmt->val.expS);
			break;
		case emptyK:
		case breakK:
		case continueK:
			break;
	}
}

void typeCASE_DECL(CASE_DECL *decl, TYPE *conditionType) {
	if (!decl) return;
	if (decl->next) typeCASE_DECL(decl->next, conditionType);

	TYPE *t;
	switch (decl->kind) {
		case caseK:
			t = typeEXP(decl->val.caseC.condition);
			if (conditionType) {
				if (t->kind != conditionType->kind) {
					printErrorType(conditionType, t, decl->loc);
				}
			} else {
				if (t->kind != type_boolK) {
					printErrorType(boolTYPE, t, decl->loc);
				}
			}
			typeSTMT(decl->val.caseC.stmt);
			break;
		case defaultK:
			typeSTMT(decl->val.defaultC);
			break;
	}
}

void typeFOR_CLAUSE(FOR_CLAUSE *decl) {
	if(decl->init_stmt) {
		typeSTMT(decl->init_stmt);
	}

	if(decl->condition) {
		TYPE *t = typeEXP(decl->condition);
		t = getDeepestType(t);
		if (t->kind != type_boolK) {
			printErrorMsg("type mismatch");
		}
	}

	if(decl->post_stmt) {
		typeSTMT(decl->post_stmt);
	}
}

void typeSHORTVAR(EXP *left, EXP *right) {
	if (left->next && right->next) typeSHORTVAR(left->next, right->next);

	left->type = typeEXP(left);
	right->type = typeEXP(right);

	/* Ignore blank id */
	if (left->val.idE->symbol->kind == blankSym) {
		return;
	}

	/* Type inference */
	if (!left->val.idE->symbol->val.type) {
		left->val.idE->symbol->val.type = right->type;
		left->type = right->type;
		if (!right->type) {
			printErrorMsg("Cannot use empty as value in assignenment");
		}
	} else {
		checkAssignable(left->type, right->type, right->loc);
	}
}

void typeAssign(EXP *left, EXP *right) {
	if (left->next && right->next) typeAssign(left->next, right->next);

	/* Ignore blank id */
	while (left->kind == parenK) {
		left = left->val.parenE;
		if (left->kind == idK) {
			if (left->val.idE->symbol->kind == blankSym) {
				return;
			}
		}
	}

	left->type = typeEXP(left);
	right->type = typeEXP(right);

	checkAssignable(left->type, right->type, right->loc);
}

TYPE *typeEXP(EXP *exp) {
	if (!exp) return NULL;
	if (exp->next) typeEXP(exp->next);

	switch (exp->kind) {
		case idK:
			exp->type = exp->val.idE->symbol->val.type;

			if (exp->val.idE->symbol->kind != varSym &&
					exp->val.idE->symbol->kind != inferredSym &&
					exp->val.idE->symbol->kind != blankSym) {
				printErrorAssign(exp->val.idE->name, exp->val.idE->loc);
			}
			break;
		case intconstK:
			exp->type = intTYPE;
			break;
		case floatconstK:
			exp->type = floatTYPE;
			break;
		case stringconstK:
			exp->type = stringTYPE;
			break;
		case rawstringconstK:
			exp->type = stringTYPE;
			break;
		case runeconstK:
			exp->type = runeTYPE;
			break;
		case plusK:
			exp->type = typeEXPplus(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case minusK:
		case timesK:
		case divK:
		case modK:
			exp->type = typeEXPnumeric(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case bitandK:
		case andnotK:
		case bitorK:
		case bitxork:
		case leftshiftK:
		case rightshiftK:
			exp->type = typeEXPbit(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case eqK:
		case neqK:
			exp->type = typeEXPcomparable(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case leK:
		case leqK:
		case geK:
		case geqK:
			exp->type = typeEXPordered(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case orK:
		case andK:
			exp->type = typeEXPbool(exp->val.binaryE.left, exp->val.binaryE.right);
			break;
		case uplusK:
		case uminusK:
			exp->type = typeEXPunumeric(exp->val.binaryE.left);
			break;
		case unotK:
			exp->type = typeEXPubool(exp->val.binaryE.left);
			break;
		case ubitnotK:
			exp->type = typeEXPuint(exp->val.binaryE.left);
			break;
		case selectorK:
			exp->type = typeEXPselector(exp->val.selectorE.exp, exp->val.selectorE.id);
			break;
		case indexK:
			exp->type = typeEXPindex(exp->val.indexE.exp, exp->val.indexE.index);
			break;
		case appendK:
			exp->type = typeEXPappend(exp->val.appendE.id, exp->val.appendE.exp);
			break;
		case funccallK:
			/* Pass the whole expression to fix mislabeled typecasts */
			exp->type = typeEXPfunccall(exp);
			break;
		case castK:
			exp->type = typeEXPcast(exp->val.castE.type, exp->val.castE.exp);
			break;
		case parenK:
			exp->type = typeEXP(exp->val.parenE);
			break;
	}
	return exp->type;
}

/* Numeric or String */
TYPE *typeEXPplus(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_stringK:
		case type_floatK:
		case type_intK:
		case type_runeK:
			if (l->kind != r->kind) {
				printErrorOperatorMismatch("+", l, r, right->loc);
			}
			break;
		default:
			printErrorOperator(l, "+", left->loc);
			break;
	}

	return left->type;
}

/* Numeric */
TYPE *typeEXPnumeric(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_intK:
		case type_floatK:
		case type_runeK:
			if (l->kind != r->kind) {
				printErrorOperatorMismatch("-, *, /, %", l, r, right->loc);
			}
			break;
		default:
			printErrorOperator(l, "-, *, /, %", left->loc);
			break;
	}

	return left->type;
}

/* Integer, Rune */
TYPE *typeEXPbit(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_intK:
		case type_runeK:
			if (r->kind != type_intK && r->kind != type_runeK) {
				printErrorOperator(r, "&, |, <<, >>, &^, ^", right->loc);
			}
			break;
		default:
			printErrorOperator(l, "&, |, <<, >>, &^, ^", left->loc);
			break;
	}

	return intTYPE;
}

TYPE *typeEXPcomparable(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_intK:
		case type_floatK:
		case type_runeK:
		case type_stringK:
		case type_boolK:
			if (l->kind != r->kind) {
				printErrorType(l, r, left->loc);
			}
			break;
		case type_structK:
			if (!isSameStruct(l->val.structT, r->val.structT)) {
				printErrorType(l, r, left->loc);
			}
			break;
		case type_refK:
		case type_arrayK:
		case type_sliceK:
			printErrorOperator(l, "==, !=", left->loc);
			break;
	}

	return boolTYPE;
}

TYPE *typeEXPordered(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_intK:
		case type_floatK:
		case type_runeK:
		case type_stringK:
			if (l->kind != r->kind) {
				printErrorType(l, r, left->loc);
			}
			break;
		case type_boolK:
		case type_structK:
		case type_refK:
		case type_arrayK:
		case type_sliceK:
			printErrorOperator(l, "<, <=, >, >=", left->loc);
			break;
	}
	return boolTYPE;
}

TYPE *typeEXPbool(EXP *left, EXP *right) {
	left->type = typeEXP(left);
	right->type = typeEXP(right);

	TYPE *l = getDeepestType(left->type);
	TYPE *r = getDeepestType(right->type);

	switch (l->kind) {
		case type_boolK:
			if (r->kind != type_boolK) {
				printErrorType(l, r, left->loc);
			}
			break;
		default:
			printErrorOperator(l, "&&, ||", left->loc);
			break;
	}
	return boolTYPE;
}
TYPE *typeEXPunumeric(EXP *exp) {
	exp->type = typeEXP(exp);

	TYPE *t = getDeepestType(exp->type);
	switch (t->kind) {
		case type_intK:
		case type_floatK:
		case type_runeK:
			break;
		default:
			printErrorOperator(t, "unary +, unary -", exp->loc);
			break;
	}

	return exp->type;
}

TYPE *typeEXPubool(EXP *exp) {
	exp->type = typeEXP(exp);

	TYPE *t = getDeepestType(exp->type);
	if (t->kind != type_boolK) {
		printErrorOperator(t, "!", exp->loc);
	}

	return boolTYPE;
}

TYPE *typeEXPuint(EXP *exp) {
	exp->type = typeEXP(exp);

	TYPE *t = getDeepestType(exp->type);
	switch (t->kind) {
		case type_intK:
		case type_runeK:
			break;
		default:
			printErrorOperator(t, "unary ^", exp->loc);
			break;
	}

	return exp->type;
}

TYPE *typeEXPselector(EXP *exp, ID *id) {
	exp->type = typeEXP(exp);


	TYPE *t = getDeepestType(exp->type);
	if (t->kind != type_structK) {
		printError("field selector require a struct", exp->loc);
	}

	/* Find type for field */
	STRUCT_DECL *decl = t->val.structT;
	ID *field = decl->id;
	while (decl) {
		while (field) {
			if (strcmp(field->name, id->name) == 0) {
				return decl->type;
			}
			field = field->next;
		}
		decl = decl->next;
		if (decl) {
			field = decl->id;
		}
	}

	/* Field not found */
	printErrorUnkownField(id, exp->loc);
	return NULL;
}

TYPE *typeEXPindex(EXP *exp, EXP *index) {
	exp->type = typeEXP(exp);
	index->type = typeEXP(index);

	if (index->type->kind != type_intK && index->type->kind != type_runeK) {
		printErrorType(intTYPE, index->type, index->loc);
	}

	/* Return enclosed type */
	switch (exp->type->kind) {
		case type_arrayK:
			exp->type = exp->type->val.arrayT.type;
			break;
		case type_sliceK:
			exp->type = exp->type->val.sliceT;
			break;
		default:
			break;
	}

	return exp->type;
}

TYPE *typeEXPappend(ID *id, EXP *exp) {
	exp->type = typeEXP(exp);

	TYPE *l = getDeepestType(id->symbol->val.type);
	if (l->kind != type_sliceK) {
		printError("first argument to append must be a slice", id->loc);
	}

	TYPE *typeOfSlice = l->val.sliceT;
	if (!isTypeAssignble(typeOfSlice, exp->type)) {
		printErrorType(typeOfSlice, exp->type, id->loc);
	}
	return id->symbol->val.type;
}

TYPE *typeEXPfunccall(EXP *call) {
	EXP *exp = call->val.funccallE.exp;
	EXP *arg = call->val.funccallE.args;

	while(exp->kind == parenK) {
		exp = exp->val.parenE;
	}

	if (exp->kind != idK) {
		printError("funtion call requires an idetifier", exp->loc);
	} else {
		/* Fix mislabeled conversions */
		if (exp->val.idE->symbol->kind == typeSym) {
			return typeEXPcast(exp->type, arg);

		} else {
			if (exp->val.idE->symbol->kind == funcSym) {
				SYMBOL *f = exp->val.idE->symbol;
				exp->type = f->val.funcSignature->type;

				if (f->val.funcSignature->arg) {
					FUNC_ARG *ref = f->val.funcSignature->arg;
					ID *id = ref->id;

					if (!isSameLength(ref, arg)) {
						printError("function call length does not match declaration", call->loc);
					}

					/* check that the types match */
					while (ref && arg) {
						while (id && arg) {
							arg->type = typeEXP(arg);
							if (ref->type->kind != arg->type->kind) {
								printErrorType(ref->type, arg->type, arg->loc);
							}
							id = id->next;
							arg = arg->next;
						}
						ref = ref->next;
						if (ref) {
							id = ref->id;
						}
					}
				}
			}
		}
	}

	return exp->type;
}

TYPE *typeEXPcast(TYPE *type, EXP *exp) {
	if (!type || !exp) return NULL;
	exp->type = typeEXP(exp);

	switch (type->kind) {
		case type_intK:
		case type_floatK:
		case type_boolK:
		case type_runeK:
			break;
		default:
			printError("conversions can only be of basic types", exp->loc);
	}

	checkCastable(type, exp->type, exp->loc);

	return type;
}

/* Structs struct must have fields in same order */
int isSameStruct(STRUCT_DECL *d1, STRUCT_DECL *d2) {
	if (!d1 && !d2) return true;
	if (d1->next && d2->next) {
		if (!isSameStruct(d1->next, d2->next)) {
			return false;
		}
		;
	}

	if (d1->type->kind != d2->type->kind) {
		return false;
	}

	ID *id1 = d1->id;
	ID *id2 = d2->id;
	while (id1 && id2) {
		if (strcmp(id1->name, id2->name) != 0) {
			return false;
		}
		id1 = id1->next;
		id2 = id2->next;
	}

	if ((id1 && !id2 ) || (!id1 && id2)) {
		return false;
	}

	return true;
}

void checkAssignable(TYPE *left, TYPE *right, YYLTYPE loc) {
	if (!left || !right) return;

	if (!isTypeAssignble(left, right)) {
		printErrorType(left, right, loc);
	}
}

int isTypeAssignble(TYPE *left, TYPE *right) {
	if (!left || !right) return false;

	switch (left->kind) {
		case type_intK:
		case type_runeK:
			if (right->kind != type_intK && right->kind != type_runeK) {
				return false;
			}
			break;
		case type_floatK:
		case type_boolK:
		case type_stringK:
			if (right->kind != left->kind) {
				return false;
			}
			break;
		case type_arrayK:
			if (right->kind != type_arrayK) {
				return false;
			} else {
				if (!isTypeAssignble(left->val.arrayT.type, right->val.arrayT.type) ||
						left->val.arrayT.size != right->val.arrayT.size) {
					return false;
				}
			}
			break;
		case type_sliceK:
			if (right->kind != type_sliceK) {
				return false;
			} else {
				if (!isTypeAssignble(left->val.sliceT, right->val.sliceT)) {
					return false;
				}
			}
			break;
		case type_structK:
			right = getDeepestType(right);
			if (right->kind != type_structK) {
				return false;
			} else {
				if (!isSameStruct(left->val.structT, right->val.structT)) {
					return false;
				}
			}
			break;
		case type_refK:
			// TODO: Check for errors
			if (right->kind != type_refK) {
				return false;
			} else {
				if (left != right) {
					false;
				}
			}
			break;
	}
	return true;
}

void checkCastable(TYPE *left, TYPE *right, YYLTYPE loc) {
	if (!left || !right) return;

	if (!isTypeCastable(left, right)) {
		printErrorType(left, right, loc);
	}
}

int isTypeCastable(TYPE *left, TYPE *right) {
	if (!left || !right) return false;

	switch (left->kind) {
		case type_floatK:
			if (right->kind != type_floatK &&
					right->kind != type_intK &&
					right->kind != type_runeK) {
				return false;
			}
			break;
		case type_intK:
		case type_runeK:
			if (right->kind != type_intK && right->kind != type_runeK) {
				return false;
			}
			break;
		case type_boolK:
			if (right->kind != type_boolK) {
				return false;
			}
			break;
		case type_stringK:
			if (right->kind != type_stringK) {
				return false;
			}
			break;
		case type_arrayK:
			if (right->kind != type_arrayK) {
				return false;
			} else {
				if (!isTypeAssignble(left->val.arrayT.type, right->val.arrayT.type) ||
						left->val.arrayT.size != right->val.arrayT.size) {
					return false;
				}
			}
			break;
		case type_sliceK:
			if (right->kind != type_sliceK) {
				return false;
			} else {
				if (!isTypeAssignble(left->val.sliceT, right->val.sliceT)) {
					return false;
				}
			}
			break;
		case type_structK:
			right = getDeepestType(right);
			if (right->kind != type_structK) {
				return false;
			} else {
				if (!isSameStruct(left->val.structT, right->val.structT)) {
					return false;
				}
			}
			break;
		case type_refK:
			break;
	}
	return true;
}

int isSameLength(FUNC_ARG *arg, EXP *exp) {
	int callLen = 0;
	int declLen = 0;

	ID *id = arg->id;
	while (arg) {
		while (id) {
			declLen++;
			id = id->next;
		}
		arg = arg->next;
		if (arg) {
			id = arg->id;
		}
	}

	while (exp) {
		callLen++;
		exp = exp->next;
	}

	return callLen == declLen;
}


int isPrintableType(EXP *exp) {
	if (!exp) return 1;
	if (exp->next) isPrintableType(exp->next);

	TYPE *t = getDeepestType(exp->type);
	switch (t->kind) {
		case type_intK:
		case type_floatK:
		case type_runeK:
		case type_stringK:
		case type_boolK:
			return 1;
		default:
			return 0;
	}
}
