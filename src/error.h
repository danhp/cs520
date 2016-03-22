#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#include "tree.h"

void yyerror(const char *msg);
void printError(char *msg, YYLTYPE loc);
void printErrorWithString(char *msg, YYLTYPE loc);
void printErrorMsg(char *msg);
void printErrorRedeclaration(YYLTYPE loc1, char *id, YYLTYPE loc2);
void printErrorUndefined(char *id, YYLTYPE loc);
void printErrorType(TYPE *t1, TYPE *t2, YYLTYPE loc);
void printErrorOperator(TYPE *type, char *op, YYLTYPE loc);
void printErrorOperatorMismatch(char *op, TYPE *left, TYPE *right, YYLTYPE loc);
void printErrorUnkownField(ID *id, YYLTYPE loc);
void printErrorAssign(char *name, YYLTYPE loc);
