D [0-9]
O [0-7]
L [a-zA-Z_]
H [a-fA-F0-9]

%{
#include "y.tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

int yycolumn = 1;
int shouldInsert = 0;
int lineno = 1;

static void update_loc() {
	yylloc.first_line = yylloc.last_line = lineno;
	yylloc.first_column = yycolumn;
	yylloc.last_column = yycolumn + yyleng - 1;
	yycolumn += yyleng;
}

#define YY_USER_ACTION update_loc();

void lexBlockComment();
int lexQuotedString();
int lexRawString();
void append_yytext(char c);
%}
%array

%%
[ \t]+          /* ignore */
\r              /* ingore */
\n              { lineno++; yycolumn = 1; if (shouldInsert) { shouldInsert = 0; return ';'; }; }
\/\/[^\n]*\n    { lineno++; yycolumn = 1; if (shouldInsert) { shouldInsert = 0; return ';'; }; }
\/\*            { lexBlockComment(); }

break           { shouldInsert = 1; return BREAK; }
case            { shouldInsert = 0; return CASE; }
chan            { shouldInsert = 0; return CHAN; }
const           { shouldInsert = 0; return CONST; }
continue        { shouldInsert = 1; return CONTINUE; }
default         { shouldInsert = 0; return DEFAULT; }
defer           { shouldInsert = 0; return DEFER; }
else            { shouldInsert = 0; return ELSE; }
fallthrough     { shouldInsert = 1; return FALLTHROUGH; }
for             { shouldInsert = 0; return FOR; }
func            { shouldInsert = 0; return FUNC; }
go              { shouldInsert = 0; return GO; }
goto            { shouldInsert = 0; return GOTO; }
if              { shouldInsert = 0; return IF; }
import          { shouldInsert = 0; return IMPORT; }
interface       { shouldInsert = 0; return INTERFACE; }
map             { shouldInsert = 0; return MAP; }
package         { shouldInsert = 0; return kPACKAGE; }
range           { shouldInsert = 0; return RANGE; }
return          { shouldInsert = 1; return RETURN; }
select          { shouldInsert = 0; return SELECT; }
struct          { shouldInsert = 0; return STRUCT; }
switch          { shouldInsert = 0; return SWITCH; }
type            { shouldInsert = 0; return kTYPE; }
var             { shouldInsert = 0; return VAR; }

int             { shouldInsert = 1; return INT; }
float64         { shouldInsert = 1; return FLOAT64; }
bool            { shouldInsert = 1; return BOOL; }
rune            { shouldInsert = 1; return RUNE; }
string          { shouldInsert = 1; return STRING; }
print           { shouldInsert = 0; return PRINT; }
println         { shouldInsert = 0; return PRINTLN; }
append          { shouldInsert = 0; return APPEND; }

"+"             { shouldInsert = 0; return '+'; }
"-"             { shouldInsert = 0; return '-'; }
"*"             { shouldInsert = 0; return '*'; }
"/"             { shouldInsert = 0; return '/'; }
"%"             { shouldInsert = 0; return '%'; }

"&"             { shouldInsert = 0; return '&'; }
"|"             { shouldInsert = 0; return '|'; }
"^"             { shouldInsert = 0; return '^'; }
"&^"            { shouldInsert = 0; return BITCLEAR; }
"<<"            { shouldInsert = 0; return LEFTSHIFT; }
">>"            { shouldInsert = 0; return RIGHTSHIFT; }

":="            { shouldInsert = 0; return VARDECL; }
"="             { shouldInsert = 0; return '='; }

"++"            { shouldInsert = 1; return INC; }
"--"            { shouldInsert = 1; return DEC; }
"..."           { shouldInsert = 0; return APPENDSLICE; }

"=="            { shouldInsert = 0; return EQ; }
"!="            { shouldInsert = 0; return NEQ; }
"<"             { shouldInsert = 0; return '<'; }
"<="            { shouldInsert = 0; return LEQ; }
">"             { shouldInsert = 0; return '>'; }
">="            { shouldInsert = 0; return GEQ; }

"&&"            { shouldInsert = 0; return AND; }
"||"            { shouldInsert = 0; return OR; }
"!"             { shouldInsert = 0; return '!'; }

"<-"            { shouldInsert = 0; return CHANDIRECTION; }

"("             { shouldInsert = 0; return '('; }
")"             { shouldInsert = 1; return ')'; }
"{"             { shouldInsert = 0; return '{'; }
"}"             { shouldInsert = 1; return '}'; }
"["             { shouldInsert = 0; return '['; }
"]"             { shouldInsert = 1; return ']'; }

","             { shouldInsert = 0; return ','; }
";"             { shouldInsert = 0; return ';'; }
"."             { shouldInsert = 0; return '.'; }
":"             { shouldInsert = 0; return ':'; }

0[xX]{H}+       { shouldInsert = 1; sscanf(yytext, "%x", &yylval.intconst); return INTCONST; }
0{O}+           { shouldInsert = 1; sscanf(yytext, "%o", &yylval.intconst); return INTCONST; }
0{D}+           { printErrorWithString("malformed octal or integer", yylloc); }
{D}+            { shouldInsert = 1; yylval.intconst = atoi(yytext); return INTCONST; }

{D}*"."{D}+     { shouldInsert = 1; yylval.floatconst = atof(yytext); return FLOATCONST; }
{D}+"."{D}*     { shouldInsert = 1; yylval.floatconst = atof(yytext); return FLOATCONST; }

\"              { if (lexQuotedString() == 0) {
                      shouldInsert = 1;
                      yylval.stringconst = (char *) malloc (strlen (yytext) + 1);
                      sprintf (yylval.stringconst, "%s", yytext);
                      return STRINGCONST;
                  }
                }

`               { if (lexRawString() == 0) {
                      shouldInsert = 1;
                      yylval.stringconst = (char *) malloc (strlen (yytext) + 1);
                      sprintf (yylval.stringconst, "%s", yytext);
                      return RAWSTRINGCONST;
                  }
                }

{L}({L}|{D})*   { shouldInsert = 1;
                  yylval.id = (char *) malloc (strlen(yytext) + 1);
                  sprintf (yylval.id, "%s", yytext);
                  return IDENTIFIER;
                }

(''')|('\\')    { printErrorWithString("unkown rune", yylloc); }
'\\a'           { shouldInsert = 1; yylval.runeconst = '\a'; return RUNECONST; }
'\\b'           { shouldInsert = 1; yylval.runeconst = '\b'; return RUNECONST; }
'\\f'           { shouldInsert = 1; yylval.runeconst = '\f'; return RUNECONST; }
'\\n'           { shouldInsert = 1; yylval.runeconst = '\n'; return RUNECONST; }
'\\r'           { shouldInsert = 1; yylval.runeconst = '\r'; return RUNECONST; }
'\\t'           { shouldInsert = 1; yylval.runeconst = '\t'; return RUNECONST; }
'\\v'           { shouldInsert = 1; yylval.runeconst = '\v'; return RUNECONST; }
'\\\\'          { shouldInsert = 1; yylval.runeconst = '\\'; return RUNECONST; }
'\\\''          { shouldInsert = 1; yylval.runeconst = '\''; return RUNECONST; }
'.'             { shouldInsert = 1; yylval.runeconst = yytext[1]; return RUNECONST; }

.               { printErrorWithString("unknown character", yylloc); }
%%

void lexBlockComment() {
	char c, c1;
	while(1) {
		while ((c = input()) != '*' && c > 0 && c != '\n') {}

		if (c == '*') {
			if ((c1 = input()) == '/') {
				/* succesfully closed comment */
				return;
			} else {
				unput(c1);
			}
		} else if (c <= 0) {
			printErrorWithString("unclosed block comment", yylloc);
			return;
		} else if (c == '\n') {
			lineno = lineno + 1;
		}
	}
}

int lexQuotedString() {
	char c, c1;

	/* 	Remove the first quote mark */
	memset(yytext, '\0', sizeof(yytext));
	while(1) {
		while ((c = input()) != '\\' && c != '"' && c > 0 && c != '\n') {
			append_yytext(c);
		}

		if (c == '\\') {
			c1 = input();
			switch (c1) {
				case 'a':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
				case 'v':
				case '\\':
				case '\'':
				case '"':
					append_yytext(c);
					append_yytext(c1);
					break;
				default:
					printErrorWithString("unkown escaped char", yylloc);
			}
		} else if (c == '\n') {
			printErrorWithString("newline in string", yylloc);
			return 1;
		} else if (c <= 0) {
			printErrorWithString("unclosed quoted string", yylloc);
			return 1;
		} else if (c == '"'){
			/* Succesfully closed string */
			return 0;
		}
	}
}

int lexRawString() {
	char c;

	/* 	Remove the first quote mark */
	memset(yytext, '\0', sizeof(yytext));
	while(1) {
		while ((c = input()) != '`' && c > 0 &&
					 c != '\a' && c != '\b' && c != '\f' &&
					 c != '\n' && c != '\r' && c != '\t' &&
					 c != '\v' && c != '\\' && c != '\'') {
			append_yytext(c);
		}

		switch (c) {
			case '\a':
				append_yytext('\\');
				append_yytext('a');
				break;
			case '\b':
				append_yytext('\\');
				append_yytext('b');
				break;
			case '\f':
				append_yytext('\\');
				append_yytext('f');
				break;
			case '\n':
				append_yytext('\\');
				append_yytext('n');
				lineno = lineno + 1;
				break;
			case '\r':
				append_yytext('\\');
				append_yytext('r');
				break;
			case '\t':
				append_yytext('\\');
				append_yytext('t');
				break;
			case '\v':
				append_yytext('\\');
				append_yytext('v');
				break;
			case '\\':
				append_yytext('\\');
				append_yytext('\\');
				break;
			case '\'':
				append_yytext('\\');
				append_yytext('\'');
				break;
			case '`':
				return 0;
		}

		if (c <= 0) {
			printErrorWithString("unclosed raw string", yylloc);
			return 1;
		}
	}
}

void append_yytext(char c){
	update_loc();
	snprintf(yytext, sizeof yytext, "%s%c", yytext, c);
}
