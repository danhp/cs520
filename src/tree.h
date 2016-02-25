#ifndef TREE_H
#define TREE_H
#include "y.tab.h"

typedef enum{intSym, floatSym, boolSym, runeSym, stringSym} SymbolKind;
typedef struct SYMBOL {
	YYLTYPE loc;
	char *id;
	SymbolKind kind;
	struct SYMBOL *next;
} SYMBOL;

typedef struct TYPE {
	YYLTYPE loc;
	enum {
		type_refK, type_intK, type_floatK, type_boolK, type_runeK, type_stringK,
		type_arrayK, type_sliceK, type_structK
	} kind;
	union {
		struct{struct ID *id; struct SYMBOL *symbol;} refT;
		struct{int size; struct TYPE *type;} arrayT;
		struct{struct TYPE *type;} sliceT;
		struct{struct STRUCT_DECL *struct_decl;} structT;
	} val;
} TYPE;

typedef struct STRUCT_DECL {
	YYLTYPE loc;
	struct ID *id;
	struct TYPE *type;
	struct STRUCT_DECL *next;
} STRUCT_DECL;

typedef struct PROGRAM {
	YYLTYPE loc;
	struct PACKAGE *package;
	struct TOP_DECL *top_decl;
} PROGRAM;

typedef struct PACKAGE {
	YYLTYPE loc;
	struct ID *id;
} PACKAGE;

typedef struct TOP_DECL {
	YYLTYPE loc;
	enum {top_varK, top_typeK, top_funcK} kind;
	union {
		struct VAR_DECL *varT;
		struct TYPE_DECL *typeT;
		struct FUNC_DECL *funcT;
	} val;
	struct TOP_DECL *next;
} TOP_DECL;

typedef struct VAR_DECL {
	YYLTYPE loc;
	struct ID *id;
	struct EXP *exp;
	struct TYPE *type;
	struct VAR_DECL *next;
} VAR_DECL;

typedef struct TYPE_DECL {
	YYLTYPE loc;
	struct ID *id;
	struct TYPE *type;
	struct TYPE_DECL *next;
} TYPE_DECL;

typedef struct FUNC_DECL {
	YYLTYPE loc;
	struct ID *id;
	struct FUNC_SIGN *signature;
	struct STMT *body;
} FUNC_DECL;

typedef struct FUNC_SIGN {
	YYLTYPE loc;
	struct FUNC_ARG *arg;
	struct TYPE *type;
} FUNC_SIGN;

typedef struct FUNC_ARG {
	YYLTYPE loc;
	struct ID *id;
	struct TYPE *type;
	struct FUNC_ARG *next;
} FUNC_ARG;

typedef struct ID {
	YYLTYPE loc;
	char *name;
	struct ID *next;
} ID;

typedef struct STMT {
	YYLTYPE loc;
	enum {
		emptyK, blockK,
		varK, typeK, printK, printlnK, shortvarK,
		returnK, ifK, ifelseK, switchK, forK, breakK, continueK,
		assignK, expK
	} kind;
	union {
		struct STMT *blockS;
		struct VAR_DECL *varS;
		struct TYPE_DECL *typeS;
		struct EXP *printS;
		struct EXP *printlnS;
		struct EXP *returnS;
		struct { struct EXP *left; struct EXP *right; } shortvarS;
		struct { struct STMT *pre_stmt; struct EXP *condition; struct STMT *body; } ifS;
		struct { struct STMT *pre_stmt; struct EXP *condition; struct STMT *thenpart; struct STMT *elsepart; } ifelseS;
		struct { struct STMT *pre_stmt; struct EXP *condition; struct CASE_DECL *body; } switchS;
		struct { struct FOR_CLAUSE *for_clause; struct STMT *body; } forS;
		struct { struct EXP *left; struct EXP *right; } assignS;
		struct EXP *expS;
	} val;
	struct STMT *next;
} STMT;

typedef struct FOR_CLAUSE {
	YYLTYPE loc;
	struct STMT *init_stmt;
	struct EXP *condition;
	struct STMT *post_stmt;
} FOR_CLAUSE;

typedef struct CASE_DECL {
	YYLTYPE loc;
	enum {caseK, defaultK} kind;
	union {
		struct {struct EXP *condition; struct STMT *stmt;} caseC;
		struct STMT *defaultC;
	} val;
	struct CASE_DECL *next;
} CASE_DECL;

typedef struct EXP {
	YYLTYPE loc;
	struct TYPE *type;
	enum {
		idK, intconstK, floatconstK, runeconstK, stringconstK, rawstringconstK,  /* literals */
		plusK, minusK, timesK, divK, modK,      /* arithmetic operators */
		bitandK, andnotK, bitorK, bitxork,      /* arithmetic operators */
		leftshiftK, rightshiftK,                /* arithmetic operators */
		eqK, neqK, leK, leqK, geK, geqK,        /* comparison operators */
		orK, andK,                              /* logical operators */
		uplusK, uminusK, unotK, ubitnotK,       /* unary operators */
		selectorK, indexK,
		funccallK, appendK, castK,
		parenK
	} kind;
	union {
		struct {struct ID *id; SYMBOL *sym;} idE;
		int intconstE;
		float floatconstE;
		char runeconstE;
		char *stringconstE;
		char *rawstringconstE;
		struct {struct EXP *left; struct EXP *right;} binaryE;
		struct EXP *unaryE;
		struct {struct EXP *exp; struct ID *id;} selectorE;
		struct {struct EXP *exp; struct EXP *index;} indexE;
		struct {struct EXP *exp; struct EXP *args;} funccallE;
		struct {struct ID *id; struct EXP *exp;} appendE;
		struct {struct TYPE *type; struct EXP *exp;} castE;
		struct EXP *parenE;
	} val;
	struct EXP *next;
} EXP;

PROGRAM *makePROGRAM(PACKAGE *package, TOP_DECL *topDecl);
PACKAGE *makePACKAGE(char *name);
ID *makeID(char *name);

TOP_DECL *makeTOPvar(VAR_DECL *varDecl);
TOP_DECL *makeTOPtype(TYPE_DECL *typeDecl);
TOP_DECL *makeTOPfunc(FUNC_DECL *funcDecl);

ID *makeID(char *name);

VAR_DECL *makeVARdecl(ID *id, TYPE *type, EXP *exp);

TYPE_DECL *makeTYPEdecl(char *name, TYPE *type);
TYPE *makeTYPEint();
TYPE *makeTYPEfloat();
TYPE *makeTYPEbool();
TYPE *makeTYPErune();
TYPE *makeTYPEstring();
TYPE *makeTYPEref(char *name);
TYPE *makeTYPEarray(int size, TYPE *type);
TYPE *makeTYPEslice(TYPE *type);
TYPE *makeTYPEstruct(STRUCT_DECL *structDecl);
STRUCT_DECL *makeSTRUCTdecl(ID *id, TYPE *type);

FUNC_DECL *makeFUNCdecl(char *name, FUNC_SIGN *signature, STMT *body);
FUNC_SIGN *makeFUNCsign(FUNC_ARG *arg, TYPE *type);
FUNC_ARG *makeFUNCarg(ID *id, TYPE *type);

STMT *makeSTMTempty();
STMT *makeSTMTblock(STMT *stmt);
STMT *makeSTMTvar(VAR_DECL *varDecl);
STMT *makeSTMTtype(TYPE_DECL *typeDecl);
STMT *makeSTMTprint(EXP *exp);
STMT *makeSTMTprintln(EXP *exp);
STMT *makeSTMTreturn(EXP *exp);
STMT *makeSTMTshortvar(EXP *left, EXP *right);
STMT *makeSMTif(STMT *pre_stmt, EXP *condition, STMT *body);
STMT *makeSMTifelse(STMT *pre_stmt, EXP *condition, STMT *thenpart, STMT *elsepart);
STMT *makeSTMTswitch(STMT *pre_stmt, EXP *condition, CASE_DECL *body);
STMT *makeSMTfor(FOR_CLAUSE *for_clause, STMT *body);
STMT *makeSTMTbreak();
STMT *makeSTMTcontinue();
STMT *makeSTMTassign(EXP *left, EXP *right);
STMT *makeSTMTexp(EXP *exp);

FOR_CLAUSE *makeFOR_CLAUSE(STMT *init_stmt, EXP *condition, STMT *post_stmt);
CASE_DECL *makeCASE_DECLcase(EXP *condition, STMT *stmt);
CASE_DECL *makeCASE_DECLdefault(STMT *stmt);

EXP *makeEXPid(char *name);
EXP *makeEXPop(EXP *left, char op, EXP *right);
EXP *makeEXPintconst(int intconst);
EXP *makeEXPfloatconst(float floatconst);
EXP *makeEXPruneconst(char runeconst);
EXP *makeEXPstringconst(char *stringconst);
EXP *makeEXPrawstringconst(char *rawstringconst);
EXP *makeEXPselector(EXP *exp, char *name);
EXP *makeEXPindex(EXP *exp, EXP *index);
EXP *makeEXPfunccall(EXP *exp, EXP *args);
EXP *makeEXPappend(char *name, EXP *exp);
EXP *makeEXPcast(TYPE *type, EXP *exp);
EXP *makeEXPparen(EXP *exp);

#endif
