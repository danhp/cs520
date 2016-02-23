%locations
%error-verbose
%{
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "error.h"

extern PROGRAM *program;
%}

%union {
	unsigned int intconst;
	float floatconst;
	char runeconst;
	char *stringconst;
	char *id;
	char charconst;

	/* AST NODES */
	struct PROGRAM *program;
	struct PACKAGE *package;
	struct TYPE *type;
	struct TOP_DECL *top_decl;
	struct VAR_DECL *var_decl;
	struct TYPE_DECL *type_decl;
	struct STRUCT_DECL *struct_decl;
	struct FUNC_DECL *func_decl;
	struct FUNC_SIGN *func_sign;
	struct FUNC_ARG *func_arg;
	struct STMT *stmt;
	struct FOR_CLAUSE *for_clause;
	struct CASE_DECL *case_decl;
	struct ID *identifier;
	struct EXP *exp;
}
%token BREAK CASE CHAN CONST CONTINUE DEFAULT DEFER
	ELSE FALLTHROUGH FOR FUNC GO GOTO IF IMPORT
	INTERFACE MAP kPACKAGE RANGE RETURN SELECT STRUCT
	SWITCH kTYPE VAR

%token INT FLOAT64 BOOL RUNE STRING
	PRINT PRINTLN APPEND

%token VARDECL
	BITCLEAR LEFTSHIFT RIGHTSHIFT
	INC DEC APPENDSLICE
	EQ NEQ LEQ GEQ AND OR NOT
	CHANDIRECTION

%token <intconst> INTCONST
%token <floatconst> FLOATCONST
%token <runeconst> RUNECONST
%token <stringconst> STRINGCONST RAWSTRINGCONST IDENTIFIER

%type <program> program
%type <package> package
%type <type> type type_lit basic_type array_type slice_type struct_type cast_type
%type <top_decl> top_decl top_decl_list
%type <var_decl> var_decl var_spec var_spec_list
%type <type_decl> type_decl type_spec type_list
%type <struct_decl> struct_decl struct_decl_list
%type <func_decl> func_decl
%type <func_sign> func_signature
%type <func_arg> arg arg_list
%type <stmt> block stmt stmt_list simple_stmt print_stmt println_stmt return_stmt
			if_stmt switch_stmt for_stmt opt_simple_stmt
%type <for_clause> for_clause
%type <case_decl> case_list switch_case
%type <identifier> id_list;
%type <exp> exp exp_list unary_exp condition prim_exp literal string_lit operand
			func_call append_exp type_cast_exp
%type <charconst> unary_op add_op mul_op assign_op

%start program
%left OR
%left AND
%left EQ NEQ '<' LEQ '>' GEQ
%left '+' '-' '|' '^'
%left '*' '/' '%' LEFTSHIFT RIGHTSHIFT '&' BITCLEAR

%%

/* 2.1 Program Structure */
program
	: package top_decl_list                        { program = makePROGRAM($1, $2); }
;

/* 2.2 Package declaration */
package
	: kPACKAGE IDENTIFIER ';'                      { $$ = makePACKAGE($2); }
;

/* 2.3 Top-level declarations */
top_decl_list
	: top_decl_list top_decl ';'                   { $$ = $2; $2->next = $1; }
	| /* empty */                                  { $$ = NULL; }
;

top_decl
	: var_decl                                     { $$ = makeTOPvar($1); }
	| type_decl                                    { $$ = makeTOPtype($1); }
	| func_decl                                    { $$ = makeTOPfunc($1); }
	| /* empty */                                  { yyerror("emtpy top-level declaration"); }
;

/* 2.4 Variable declaration */
var_decl
	: VAR var_spec                                 { $$ = $2; }
	| VAR '(' var_spec_list opt_semi ')'           { $$ = $3; }
	| VAR '(' ')'                                  { $$ = NULL; }
;

var_spec_list
	: var_spec_list ';' var_spec                   { $$ = $3; $3->next = $1; }
	| var_spec                                     { $$ = $1; }
;

var_spec
	: id_list type                                 { $$ = makeVARdecl($1, $2, NULL); }
	| id_list '=' exp_list                         { $$ = makeVARdecl($1, NULL, $3); }
	| id_list type '=' exp_list                    { $$ = makeVARdecl($1, $2, $4); }
;

id_list
	: id_list ',' IDENTIFIER                       { $$ = makeID($3); $$->next = $1; }
	| IDENTIFIER                                   { $$ = makeID($1); }
;

/* 2.5 Type declaration & 2.7 Types */
type_decl
	: kTYPE type_spec                              { $$ = $2; }
	| kTYPE '(' type_list opt_semi ')'             { $$ = $3; }
	| kTYPE '(' ')'                                { $$ = NULL; }
;

type_list
	: type_list ';' type_spec                      { $$ = $3; $3->next = $1; }
	| type_spec                                    { $$ = $1; }
;

type_spec
	: IDENTIFIER type                              { $$ = makeTYPEdecl($1, $2); }

type
	: '(' type ')'                                 { $$ = $2; }
	| type_lit                                     { $$ = $1; }
	| IDENTIFIER                                   { $$ = makeTYPEref($1); }
;

type_lit
	: basic_type                                   { $$ = $1; }
	| array_type                                   { $$ = $1; }
	| slice_type                                   { $$ = $1; }
	| struct_type                                  { $$ = $1; }
;

basic_type
	: INT                                          { $$ = makeTYPEint(); }
	| FLOAT64                                      { $$ = makeTYPEfloat(); }
	| BOOL                                         { $$ = makeTYPEbool(); }
	| RUNE                                         { $$ = makeTYPErune(); }
	| STRING                                       { $$ = makeTYPEstring(); }
;

array_type
	: '[' INTCONST ']' type                        { $$ = makeTYPEarray($2, $4); }
;

slice_type
	: '[' ']' type                                 { $$ = makeTYPEslice($3); }
;

struct_type
	: STRUCT '{'  struct_decl_list opt_semi '}'    { $$ = makeTYPEstruct($3); }
	| STRUCT '{' '}'                               { $$ = makeTYPEstruct(NULL); }
;

struct_decl_list
	: struct_decl_list ';' struct_decl             { $$ = $3; $$->next = $1; }
	| struct_decl                                  { $$ = $1; }
;

struct_decl
	: id_list type                                 { $$ = makeSTRUCTdecl($1, $2); }
;

/* 2.6 Function declarations */
func_decl
	: FUNC IDENTIFIER func_signature block         { $$ = makeFUNCdecl($2, $3, $4); }
;

func_signature
	: '(' arg_list ')' type                        { $$ = makeFUNCsign($2, $4); }
	| '(' arg_list ')'                             { $$ = makeFUNCsign($2, NULL); }
;

arg_list
	: arg_list ',' arg                             { $$ = $3; $3->next = $1; }
	| arg                                          { $$ = $1; }
	| /* empty */                                  { $$ = NULL; }
;

arg
	: id_list type                                 { $$ = makeFUNCarg($1, $2); }

;

block
	: '{' stmt_list '}'                            { $$ = $2; }

/* 2.8 Statements */
stmt_list
	: stmt_list ';' stmt               { $$ = $3; $$->next = $1; }
	| stmt                             { $$ = $1; }
;

stmt: simple_stmt                      { $$ = $1; }
	| var_decl                         { $$ = makeSTMTvar($1); }
	| type_decl                        { $$ = makeSTMTtype($1); }
	| block                            { $$ = $1; }
	| print_stmt                       { $$ = $1; }
	| println_stmt                     { $$ = $1; }
	| return_stmt                      { $$ = $1; }
	| if_stmt                          { $$ = $1; }
	| switch_stmt                      { $$ = $1; }
	| for_stmt                         { $$ = $1; }
	| BREAK                            { $$ = makeSTMTbreak(); }
	| CONTINUE                         { $$ = makeSTMTcontinue(); }
	| /* empty */                      { $$ = makeSTMTempty(); }
;

simple_stmt
	: exp_list VARDECL exp_list        { $$ = makeSTMTshortvar($1, $3); }
	| exp_list '=' exp_list            { $$ = makeSTMTassign($1, $3); }
	| exp assign_op exp                { $$ = makeSTMTassign($1, makeEXPop($1, $2, $3)); }
	| exp INC                          { $$ = makeSTMTassign($1, makeEXPop($1, '+', makeEXPintconst(1))); }
	| exp DEC                          { $$ = makeSTMTassign($1, makeEXPop($1, '-', makeEXPintconst(1))); }
	| exp                              { $$ = makeSTMTexp($1); }
;

assign_op
	: add_op '='                       { $$ = $1; }
	| mul_op '='                       { $$ = $1; }
;

print_stmt
	: PRINT '(' exp_list ')'           { $$ = makeSTMTprint($3); }
	| PRINT '(' ')'                    { $$ = makeSTMTprint(NULL); }
;

println_stmt
	: PRINTLN '(' exp_list ')'         { $$ = makeSTMTprintln($3); }
	| PRINTLN '(' ')'                  { $$ = makeSTMTprintln(NULL); }
;

return_stmt
	: RETURN exp                       { $$ = makeSTMTreturn($2); }
	| RETURN                           { $$ = makeSTMTreturn(NULL); }
;

if_stmt
	: IF simple_stmt ';' exp block                           { $$ = makeSMTif($2, $4, $5); }
	| IF simple_stmt ';' exp block ELSE if_stmt              { $$ = makeSMTifelse($2, $4, $5, $7); }
	| IF simple_stmt ';' exp block ELSE block                { $$ = makeSMTifelse($2, $4, $5, $7); }
	| IF exp block                                           { $$ = makeSMTif(NULL, $2, $3); }
	| IF exp block ELSE if_stmt                              { $$ = makeSMTifelse(NULL, $2, $3, $5); }
	| IF exp block ELSE block                                { $$ = makeSMTifelse(NULL, $2, $3, $5); }
;

switch_stmt
	: SWITCH simple_stmt ';' exp '{' case_list '}'           { $$ = makeSTMTswitch($2, $4, $6); }
	| SWITCH simple_stmt ';' '{' case_list '}'               { $$ = makeSTMTswitch($2, NULL, $5); }
	| SWITCH exp '{' case_list '}'                           { $$ = makeSTMTswitch(NULL, $2, $4); }
	| SWITCH '{' case_list '}'                               { $$ = makeSTMTswitch(NULL, NULL, $3); }
;

case_list
	: case_list switch_case                                  { $$ = $2; $2->next = $1; }
	| /* empty */                                            { $$ = NULL; }
;
switch_case
	: CASE exp_list ':' stmt_list                            { $$ = makeCASE_DECLcase($2, $4); }
	| DEFAULT ':' stmt_list                                  { $$ = makeCASE_DECLdefault($3); }
;

for_stmt
	: FOR for_clause block                                   { $$ = makeSMTfor($2, $3); }
;

for_clause
	: opt_simple_stmt ';' condition ';' opt_simple_stmt      { $$ = makeFOR_CLAUSE($1, $3, $5); }
	| condition                                              { $$ = makeFOR_CLAUSE(NULL, $1, NULL); }
;

condition
	: exp                        { $$ = $1; }
	| /* empty */                { $$ = NULL; }
;

opt_simple_stmt
	: simple_stmt                { $$ = $1; }
	| /* empty */                { $$ = NULL; }
;

/* 2.9 Expressions */
exp_list
	: exp_list ',' exp           { $$ = $3; $3->next = $1; }
	| exp                        { $$ = $1; }
;

exp
	: unary_exp                  { $$ = $1; }
	| exp '+' exp                { $$ = makeEXPop($1, '+', $3); }
	| exp '-' exp                { $$ = makeEXPop($1, '-', $3); }
	| exp '*' exp                { $$ = makeEXPop($1, '*', $3); }
	| exp '/' exp                { $$ = makeEXPop($1, '/', $3); }
	| exp '%' exp                { $$ = makeEXPop($1, '%', $3); }
	| exp '&' exp                { $$ = makeEXPop($1, '&', $3); }
	| exp '|' exp                { $$ = makeEXPop($1, '|', $3); }
	| exp '^' exp                { $$ = makeEXPop($1, '^', $3); }
	| exp BITCLEAR exp           { $$ = makeEXPop($1, 'b', $3); }
	| exp LEFTSHIFT exp          { $$ = makeEXPop($1, 'l', $3); }
	| exp RIGHTSHIFT exp         { $$ = makeEXPop($1, 'r', $3); }

	| exp EQ exp                 { $$ = makeEXPop($1, '=', $3); }
	| exp NEQ exp                { $$ = makeEXPop($1, 'n', $3); }
	| exp '<' exp                { $$ = makeEXPop($1, '<', $3); }
	| exp LEQ exp                { $$ = makeEXPop($1, 's', $3); }
	| exp '>' exp                { $$ = makeEXPop($1, '>', $3); }
	| exp GEQ exp                { $$ = makeEXPop($1, 'g', $3); }

	| exp OR exp                 { $$ = makeEXPop($1, 'o', $3); }
	| exp AND exp                { $$ = makeEXPop($1, 'a', $3); }
;


unary_exp
	: unary_op unary_exp         { $$ = makeEXPop($2, $1, NULL); }
	| prim_exp                   { $$ = $1; }
;

prim_exp
	: operand                    { $$ = $1; }
	| prim_exp '.' IDENTIFIER    { $$ = makeEXPselector($1, $3); }
	| prim_exp '[' INTCONST ']'  { $$ = makeEXPindex($1, $3); }
	| func_call                  { $$ = $1; }
	| append_exp                 { $$ = $1; }
	| type_cast_exp              { $$ = $1; }
;

operand
	: literal                    { $$ = $1; }
	| IDENTIFIER                 { $$ = makeEXPid($1); }
	| '(' exp ')'                { $$ = $2; }
;

literal
	: INTCONST                   { $$ = makeEXPintconst($1); }
	| FLOATCONST                 { $$ = makeEXPfloatconst($1); }
	| RUNECONST                  { $$ = makeEXPruneconst($1); }
	| string_lit                 { $$ = $1; }
;

string_lit
	: STRINGCONST                { $$ = makeEXPstringconst($1); }
	| RAWSTRINGCONST             { $$ = makeEXPstringconst($1); }
;

/* OPERATORS */
unary_op
	: '+'                        { $$ = '+'; }
	| '-'                        { $$ = '-'; }
	| '!'                        { $$ = '!'; }
	| '^'                        { $$ = '^'; }
	| '*'                        { yyerror("unsupported unary operator"); }
	| '&'                        { yyerror("unsupported unary operator"); }
	| CHANDIRECTION              { yyerror("unsupported unary operator"); }
;

add_op
	: '+'                        { $$ = '+'; }
	| '-'                        { $$ = '-'; }
	| '|'                        { $$ = '|'; }
	| '^'                        { $$ = '^'; }
;

mul_op
	: '*'                        { $$ = '*'; }
	| '/'                        { $$ = '/'; }
	| '%'                        { $$ = '*'; }
	| LEFTSHIFT                  { $$ = 'l'; }
	| RIGHTSHIFT                 { $$ = 'r'; }
	| '&'                        { $$ = '&'; }
	| BITCLEAR                   { $$ = 'b'; }
;

/* 2.9.6 Function Calls */
/* Allowed for nested parens for ID but now we need to weed */
func_call
	: operand '(' exp_list ')'             { $$ = makeEXPfunccall($1, $3); }
;

/* 2.9.7 Append */
append_exp
	: APPEND '(' IDENTIFIER ',' exp ')'   { $$ = makeEXPappend($3, $5); }
;

/* 2.9.8 Type casts */
/* cast to base types supported here, aliases will look like function calls until typing */
type_cast_exp
	: cast_type '(' exp ')'               { $$ = makeEXPcast($1, $3); }
;

cast_type
	: '(' cast_type ')'                   { $$ = $2; }
	| basic_type                          { $$ = $1; }
;

opt_semi
	: ';'
	| /* empty */
;
%%
