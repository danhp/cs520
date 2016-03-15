#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"

void yyerror(const char *msg);
void printError(char *msg, YYLTYPE loc);
void printErrorWithString(char *msg, YYLTYPE loc);
void printErrorMsg(char *msg);
void printErrorRedeclaration(YYLTYPE loc1, char *id, YYLTYPE loc2);
void printErrorUndefined(char *id, YYLTYPE loc);
