#include "error.h"

extern char yytext[];

void yyerror(const char *msg) {
	fprintf (stderr, "ERROR, %d:%d, %s at %s\n", yylloc.first_line, yylloc.first_column, msg, yytext);
	exit(1);
}

void printError(char *msg, YYLTYPE loc) {
	fprintf(stderr, "ERROR, %d:%d, %s at %s\n", loc.first_line, loc.first_column, msg, yytext);
	exit(1);
}
void printErrorMsg(char *msg)
{
	fprintf(stderr, "ERROR %s\n", msg);
	exit(1);
}
