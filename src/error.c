#include "error.h"

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
