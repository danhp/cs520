#include "error.h"
#include "pretty.h"

void pTYPE(TYPE *obj);
void pID(ID *obj);
void printSTRUCT_DECL(STRUCT_DECL *obj);

extern char yytext[];
extern char *filename;

void yyerror(const char *msg) {
	fprintf (stderr, "%s: ERROR, %d:%d, %s at %s\n", filename, yylloc.first_line, yylloc.first_column, msg, yytext);
	exit(1);
}

void printError(char *msg, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, %s\n", filename, loc.first_line, loc.first_column, msg);
	exit(1);
}
void printErrorWithString(char *msg, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, %s at %s\n", filename, loc.first_line, loc.first_column, msg, yytext);
	exit(1);
}

void printErrorMsg(char *msg) {
	fprintf(stderr, "%s: ERROR %s\n", filename, msg);
	exit(1);
}

void printErrorRedeclaration(YYLTYPE loc1, char *id, YYLTYPE loc2) {
	fprintf(stderr, "%s: ERROR, %d:%d, %s redeclared in this block, previous declaration at %d:%d\n", filename, loc1.first_line, loc1.first_column, id, loc2.first_line, loc2.first_column);
	exit(1);
}

void printErrorUndefined(char *id, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, undefined: %s\n", filename, loc.first_line, loc.first_column, id);
	exit(1);
}

void printErrorType(TYPE *t1, TYPE *t2, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, expected ", filename, loc.first_line, loc.first_column);
	pTYPE(t1);
	fprintf(stderr, "%s", ", found ");
	pTYPE(t2);
	fprintf(stderr, "\n");
	exit(1);
}

void printErrorOperator(TYPE *type, char *op, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, operator %s not defined on ", filename, loc.first_line, loc.first_column, op);
	pTYPE(type);
	fprintf(stderr, "\n");
	exit(1);
}

void printErrorUnkownField(ID *id, YYLTYPE loc) {
	fprintf(stderr, "%s: ERROR, %d:%d, struct has no field named ", filename, loc.first_line, loc.first_column);
	pID(id);
	fprintf(stderr, "\n");
	exit(1);
}

void pTYPE(TYPE *obj) {
	if(!obj) return;

	switch(obj->kind) {
		case type_refK:
			return;
		case type_intK:
			fprintf(stderr, "%s", "int");
			return;
		case type_floatK:
			fprintf(stderr, "%s", "float64");
			break;
		case type_boolK:
			fprintf(stderr, "%s", "bool");
			break;
		case type_runeK:
			fprintf(stderr, "%s", "rune");
			break;
		case type_stringK:
			fprintf(stderr, "%s", "string");
			return;
		case type_arrayK:
			fprintf(stderr, "%s", "[");
			fprintf(stderr, "%d", obj->val.arrayT.size);
			fprintf(stderr, "%s", "]");
			if(obj->val.arrayT.type) {
				pTYPE(obj->val.arrayT.type);
			}
			break;
		case type_sliceK:
			fprintf(stderr, "%s", "[]");
			if(obj->val.sliceT) {
				pTYPE(obj->val.sliceT);
			}
			break;
		case type_structK:
			fprintf(stderr, "%s", "struct {");
			if(obj->val.structT) {
				printSTRUCT_DECL(obj->val.structT);
			}
			fprintf(stderr, "%s", "}");
			return;
	}
}

void pID(ID *obj) {
	if (obj->next != NULL) {
		pID(obj->next);
		fprintf(stderr, "%s", ", ");
	}
	fprintf(stderr, "%s", obj->name);
}

void printSTRUCT_DECL(STRUCT_DECL *obj) {
	if (!obj) return;
	if(obj->next) {
		printSTRUCT_DECL(obj->next);
	}

	pID(obj->id);
	if(obj->type) {
		fprintf(stderr, "%s", " ");
		pTYPE(obj->type);
	}
	fprintf(stderr, "%s", "; ");
}
