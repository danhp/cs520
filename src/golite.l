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
void append_yytext(char c);
%}
%array

%%
[ \t]+          /* ignore */
\r              { shouldInsert = 0; yycolumn = 1; }
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
"!"             { shouldInsert = 0; return NOT; }

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

`[^`]*`         { shouldInsert = 1;
                  yylval.stringconst = (char *) malloc (strlen (yytext) + 1);
                  sprintf (yylval.stringconst, "%s", yytext);
                  return RAWSTRINGCONST;
                }

{L}({L}|{D})*   { shouldInsert = 1;
                  yylval.id = (char *) malloc (strlen(yytext) + 1);
                  sprintf (yylval.id, "%s", yytext);
                  return IDENTIFIER;
                }

(''')|('\\')    { printError("unkown rune", yylloc); }
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

`[^`]*$         { printError("unclosed raw string", yylloc); }
.               { printError("unknown character", yylloc); }
%%

void lexBlockComment() {
	char c, c1;
	while(1) {
		while ((c = input()) != '*' && c > 0) {}

		if (c == '*') {
			if ((c1 = input()) == '/') {
				/* succesfully closed comment */
				return;
			} else {
				unput(c1);
			}
		} else if (c <= 0) {
			printError("unclosed block comment", yylloc);
			return;
		}
	}
}

int lexQuotedString() {
	char c, c1;

	while(1) {
		while ((c = input()) != '\\' && c != '"' && c != 0 && c != '\n') {
			append_yytext(c);
		}

		append_yytext(c);
		if (c == '\\') {
			c1 = input();
			append_yytext(c1);
			if (c1 != 'a' && c1 != 'b' &&
						c1 != 'f' && c1 != 'n' && c1 != 'r' && c1 != 't' &&
						c1 != 'v' && c1 != '\\' && c1 != '"') {
				printError("unkown escaped char", yylloc);
				return 1;
			}
		} else if (c == '\n') {
			printError("newline in string", yylloc);
			return 1;
		} else if (c == 0) {
			printError("unclosed quoted string", yylloc);
			return 1;
		} else if (c == '"'){
			/* Succesfully closed comment */
			return 0;
		}
	}
}

void append_yytext(char c){
	update_loc();
	snprintf(yytext, sizeof yytext, "%s%c", yytext, c);
}
