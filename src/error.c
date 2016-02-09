#include "error.h"

extern char yytext[];

void yyerror(char *msg) {
	printf ("ERROR, %d:%d, %s at %s\n", yylloc.first_line, yylloc.first_column, msg, yytext);
	exit(1);
}

void printError(char *msg, YYLTYPE loc) {
	printf("ERROR, %d:%d, %s at %s\n", loc.first_line, loc.first_column, msg, yytext);
	exit(1);
}
