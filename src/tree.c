#include <stdio.h>

#include "tree.h"
#include "memory.h"

extern YYLTYPE yylloc;

PROGRAM *makePROGRAM(PACKAGE *package, TOP_DECL *topDecl) {
	PROGRAM *p;
	p = NEW(PROGRAM);
	p->loc = yylloc;
	p->package = package;
	p->top_decl = topDecl;
	return p;
}

PACKAGE *makePACKAGE(char *name) {
	PACKAGE *p;
	p = NEW(PACKAGE);
	p->loc = yylloc;
	p->id = makeID(name);
	return p;
}

TOP_DECL *makeTOPvar(VAR_DECL *varDecl) {
	TOP_DECL *tp;
	tp = NEW(TOP_DECL);
	tp->loc = yylloc;
	tp->kind = top_varK;
	tp->val.varT = varDecl;
	return tp;
}

TOP_DECL *makeTOPtype(TYPE_DECL *typeDecl) {
	TOP_DECL *tp;
	tp = NEW(TOP_DECL);
	tp->loc = yylloc;
	tp->kind = top_typeK;
	tp->val.typeT = typeDecl;
	return tp;
}

TOP_DECL *makeTOPfunc(FUNC_DECL *funcDecl) {
	TOP_DECL *tp;
	tp = NEW(TOP_DECL);
	tp->loc = yylloc;
	tp->kind = top_funcK;
	tp->val.funcT = funcDecl;
	return tp;
}

ID *makeID(char *name) {
	ID *id;
	id = NEW(ID);
	id->loc = yylloc;
	id->name = name;
	return id;
}

VAR_DECL *makeVARdecl(ID *id, TYPE *type, EXP *exp) {
	VAR_DECL *v;
	v = NEW(VAR_DECL);
	v->loc = yylloc;
	v->id = id;
	v->type = type;
	v->exp = exp;
	return v;
}

TYPE_DECL *makeTYPEdecl(char *name, TYPE *type) {
	TYPE_DECL *t;
	t = NEW(TYPE_DECL);
	t->loc = yylloc;
	t->id = makeID(name);
	t->type = type;
	return t;
}

TYPE *makeTYPEint() {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_intK;
	return t;
}
TYPE *makeTYPEfloat() {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_floatK;
	return t;
}

TYPE *makeTYPEbool() {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_boolK;
	return t;
}

TYPE *makeTYPErune() {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_runeK;
	return t;
}

TYPE *makeTYPEstring() {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_stringK;
	return t;
}

TYPE *makeTYPEref(char *name) {
	TYPE *t;

	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_refK;
	t->val.refT = makeID(name);
	return t;
}

TYPE *makeTYPEarray(int size, TYPE *type) {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_arrayK;
	t->val.arrayT.size = size;
	t->val.arrayT.type = type;
	return t;
}

TYPE *makeTYPEslice(TYPE *type) {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_sliceK;
	t->val.sliceT = type;
	return t;
}

TYPE *makeTYPEstruct(STRUCT_DECL *structDecl) {
	TYPE *t;
	t = NEW(TYPE);
	t->loc = yylloc;
	t->kind = type_structK;
	t->val.structT = structDecl;
	return t;
}

STRUCT_DECL *makeSTRUCTdecl(ID *id, TYPE *type) {
	STRUCT_DECL *s;
	s = NEW(STRUCT_DECL);
	s->loc = yylloc;
	s->id = id;
	s->type = type;
	return s;
}

FUNC_DECL *makeFUNCdecl(char *name, FUNC_SIGN *signature, STMT *body) {
	FUNC_DECL *f;
	f = NEW(FUNC_DECL);
	f->loc = yylloc;
	f->id = makeID(name);
	f->signature = signature;
	f->body = body;
	return f;
}

FUNC_SIGN *makeFUNCsign(FUNC_ARG *arg, TYPE *type) {
	FUNC_SIGN *fs;
	fs = NEW(FUNC_SIGN);
	fs->loc = yylloc;
	fs->arg = arg;
	fs->type = type;
	return fs;
}

FUNC_ARG *makeFUNCarg(ID *id, TYPE *type) {
	FUNC_ARG *fa;
	fa = NEW(FUNC_ARG);
	fa->loc = yylloc;
	fa->id = id;
	fa->type = type;
	return fa;
}

STMT *makeSTMTempty() {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = emptyK;
	return s;
}

STMT *makeSTMTblock(STMT *stmt) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	if (stmt) {
		s->kind = blockK;
		s->val.blockS = stmt;
	} else {
		s->kind = emptyK;
	}
	return s;
}

STMT *makeSTMTvar(VAR_DECL *varDecl) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	if (varDecl) {
		s->kind = varK;
		s->val.varS = varDecl;
	} else {
		s->kind = emptyK;
	}
	return s;
}

STMT *makeSTMTtype(TYPE_DECL *typeDecl) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	if (typeDecl) {
		s->kind = typeK;
		s->val.typeS = typeDecl;
	} else {
		s->kind = emptyK;
	}
	return s;
}

STMT *makeSTMTprint(EXP *exp) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = printK;
	s->val.printS = exp;
	return s;
}

STMT *makeSTMTprintln(EXP *exp) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = printlnK;
	s->val.printlnS = exp;
	return s;
}

STMT *makeSTMTreturn(EXP *exp) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = returnK;
	s->val.returnS = exp;
	return s;
}

STMT *makeSTMTshortvar(EXP *left, EXP *right) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = shortvarK;
	s->val.shortvarS.left = left;
	s->val.shortvarS.right = right;
	return s;
}

STMT *makeSMTif(STMT *pre_stmt, EXP *condition, STMT *body) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = ifK;
	s->val.ifS.pre_stmt = pre_stmt;
	s->val.ifS.condition = condition;
	s->val.ifS.body = body;
	return s;
}

STMT *makeSMTifelse(STMT *pre_stmt, EXP *condition, STMT *thenpart, STMT *elsepart) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = ifelseK;
	s->val.ifelseS.pre_stmt = pre_stmt;
	s->val.ifelseS.condition = condition;
	s->val.ifelseS.thenpart = thenpart;
	s->val.ifelseS.elsepart = elsepart;
	return s;
}

STMT *makeSTMTswitch(STMT *pre_stmt, EXP *condition, CASE_DECL *body) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = switchK;
	s->val.switchS.pre_stmt = pre_stmt;
	s->val.switchS.condition = condition;
	s->val.switchS.body = body;
	return s;
}

STMT *makeSMTfor(FOR_CLAUSE *for_clause, STMT *body) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = forK;
	s->val.forS.for_clause = for_clause;
	s->val.forS.body = body;
	return s;
}

STMT *makeSTMTbreak() {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = breakK;
	return s;
}

STMT *makeSTMTcontinue() {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = continueK;
	return s;
}

STMT *makeSTMTassign(EXP *left, EXP *right) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = assignK;
	s->val.assignS.left = left;
	s->val.assignS.right = right;
	return s;
}

STMT *makeSTMTexp(EXP *exp) {
	STMT *s;
	s = NEW(STMT);
	s->loc = yylloc;
	s->kind = expK;
	s->val.expS = exp;
	return s;
}

FOR_CLAUSE *makeFOR_CLAUSE(STMT *init_stmt, EXP *condition, STMT *post_stmt) {
	FOR_CLAUSE *fc;
	fc = NEW(FOR_CLAUSE);
	fc->loc = yylloc;
	fc->init_stmt = init_stmt;
	fc->condition = condition;
	fc->post_stmt = post_stmt;
	return fc;
}

CASE_DECL *makeCASE_DECLcase(EXP *condition, STMT *stmt) {
	CASE_DECL *cd;
	cd = NEW(CASE_DECL);
	cd->loc = yylloc;
	cd->kind = caseK;
	cd->val.caseC.condition = condition;
	cd->val.caseC.stmt = stmt;
	return cd;
}

CASE_DECL *makeCASE_DECLdefault(STMT *stmt) {
	CASE_DECL *cd;
	cd = NEW(CASE_DECL);
	cd->loc = yylloc;
	cd->kind = defaultK;
	cd->val.defaultC = stmt;
	return cd;
}

EXP *makeEXPid(char *name) {
	EXP *e;
	/* SYMBOL *s; */

	e = NEW(EXP);
	e->kind = idK;
	e->loc = yylloc;

	/* s = NEW(SYMBOL); */
	/* s->id= name; */
	/* e->val.idE.sym = s; */
	e->val.idE = makeID(name);
	return e;
}


EXP *makeEXPop(EXP *left, char op, EXP *right) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;

	if (right) {
		e->val.binaryE.left = left;
		e->val.binaryE.right = right;
		switch (op) {
		case '+':
			e->kind = plusK;
			break;
		case '-':
			e->kind = minusK;
			break;
		case '*':
			e->kind = timesK;
			break;
		case '/':
			e->kind = divK;
			break;
		case '%':
			e->kind = modK;
			break;
		case '&':
			e->kind = bitandK;
			break;
		case '|':
			e->kind = bitorK;
			break;
		case '^':
			e->kind = bitxork;
			break;
		case 'b':
			e->kind = andnotK;
			break;
		case 'l':
			e->kind = leftshiftK;
			break;
		case 'r':
			e->kind = rightshiftK;
			break;
		case '=':
			e->kind = eqK;
			break;
		case 'n':
			e->kind = neqK;
			break;
		case '<':
			e->kind = leK;
			break;
		case 's':
			e->kind = leqK;
			break;
		case '>':
			e->kind = geK;
			break;
		case 'g':
			e->kind = geqK;
			break;
		case 'o':
			e->kind = orK;
			break;
		case 'a':
			e->kind = andK;
			break;
		}
	} else {
		e->val.unaryE = left;
		switch(op) {
		case '+':
			e->kind = uplusK;
			break;
		case '-':
			e->kind = uminusK;
			break;
		case '!':
			e->kind = unotK;
			break;
		case '^':
			e->kind = ubitnotK;
			break;
		}
	}

	return e;
}

EXP *makeEXPintconst(int intconst) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = intconstK;
	e->val.intconstE = intconst;
	return e;
}

EXP *makeEXPfloatconst(float floatconst) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = floatconstK;
	e->val.floatconstE = floatconst;
	return e;
}

EXP *makeEXPruneconst(char runeconst) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = runeconstK;
	e->val.runeconstE = runeconst;
	return e;
}

EXP *makeEXPstringconst(char *stringconst) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = stringconstK;
	e->val.stringconstE = stringconst;
	return e;
}

EXP *makeEXPrawstringconst(char *rawstringconst) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = rawstringconstK;
	e->val.rawstringconstE = rawstringconst;
	return e;
}

EXP *makeEXPselector(EXP *exp, char *name) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = selectorK;
	e->val.selectorE.exp = exp;
	e->val.selectorE.id = makeID(name);
	return e;
}
EXP *makeEXPindex(EXP *exp, EXP *index) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = indexK;
	e->val.indexE.exp = exp;
	e->val.indexE.index = index;
	return e;
}
EXP *makeEXPfunccall(EXP *exp, EXP *args) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = funccallK;
	e->val.funccallE.exp = exp;
	e->val.funccallE.args = args;
	return e;
}
EXP *makeEXPappend(char *name, EXP *exp) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = appendK;
	e->val.appendE.id = makeID(name);
	e->val.appendE.exp = exp;
	return e;
}
EXP *makeEXPcast(TYPE *type, EXP *exp) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = castK;
	e->val.castE.type = type;
	e->val.castE.exp = exp;
	return e;
}

EXP *makeEXPparen(EXP *exp) {
	EXP *e;
	e = NEW(EXP);
	e->loc = yylloc;
	e->kind = parenK;
	e->val.parenE = exp;
	return e;
}
