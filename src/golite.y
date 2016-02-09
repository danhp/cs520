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
	struct PROGRAM *program;

	int intconst;
	float floatconst;
	int boolconst;
	char runeconst;
	char *stringconst;
	char *id;
}
%token BREAK CASE CHAN CONST CONTINUE DEFAULT DEFER
	ELSE FALLTHROUGH FOR FUNC GO GOTO IF IMPORT
	INTERFACE MAP PACKAGE RANGE RETURN SELECT STRUCT
	SWITCH TYPE VAR

%token INT FLOAT64 BOOL RUNE STRING
	PRINT PRINTLN APPEND

%token VARDECL
	BITCLEAR LEFTSHIFT RIGHTSHIFT
	INC DEC APPENDSLICE
	EQ NEQ LEQ GEQ AND OR NOT
	CHANDIRECTION

%token <intconst> INTCONST
%token <floatconst> FLOATCONST
%token <boolconst> BOOLCONST
%token <charconst> RUNECONST
%token <stringconst> STRINGCONST RAWSTRINGCONST IDENTIFIER

%start program

%left OR
%left AND
%left EQ NEQ '<' LEQ '>' GEQ
%left '+' '-' '|' '^'
%left '*' '/' '%' LEFTSHIFT RIGHTSHIFT '&' BITCLEAR

%%

/* 2.1 Program Structure */
program: package top_decl_list
;

/* 2.2 Package declaration */
package: PACKAGE IDENTIFIER ';'
;

/* 2.3 Top-level declarations */
top_decl_list: top_decl_list top_decl ';'
	| /* empty */
;

top_decl: var_decl
	| type_decl
	| func_decl
	| /* empty */
		{ yyerror("emtpy top-level declaration"); }
;

/* 2.4 Variable declaration */
var_decl: VAR var_spec
	| VAR '(' var_spec_list opt_semi ')' /* distribute var keyword */
	| VAR '(' ')'
;

var_spec_list: var_spec
	| var_spec_list ';' var_spec
;

var_spec: id_list type
	| id_list '=' exp_list
	| id_list type '=' exp_list
;

id_list: IDENTIFIER
	| id_list ',' IDENTIFIER
;

/* 2.5 Type declaration & 2.7 Types */
type_decl: TYPE type_spec
	| TYPE '(' type_list opt_semi ')'
	| TYPE '(' ')'
;

type_list: type_list ';' type_spec
	| type_spec
;

type_spec: IDENTIFIER type

type: IDENTIFIER | type_lit | '(' type ')'
;

type_lit: basic_type
	| array_type
	| slice_type
	| struct_type
;

basic_type: INT
	| FLOAT64
	| BOOL
	| RUNE
	| STRING
;

array_type: '[' INTCONST ']' type
;

slice_type: '[' ']' type
;

struct_type: STRUCT '{' '}'
           | STRUCT '{'  id_list type ';' '}'
;

/* 2.6 Function declarations */
func_decl: FUNC IDENTIFIER func_signature block
;

func_signature: '(' arg_list ')' type
	| '(' arg_list ')'
;

arg_list: arg
	| arg_list ',' arg
;

arg: id_list type
	| /* empty */
;

block: '{' stmt_list '}'

/* 2.8 Statements */
stmt_list: stmt_list ';' stmt
	| stmt
;

stmt: simple_stmt
	| var_decl
	| type_decl
	| block
	| print_stmt
	| println_stmt
	| return_stmt
	| if_stmt
	| switch_stmt
	| for_stmt
	| break_stmt
	| continue_stmt
	| /* empty */
;

simple_stmt: exp
	| exp INC
	| exp DEC
	| exp assign_op exp
	| exp_list '=' exp_list
	| exp_list VARDECL exp_list
;

assign_op: add_op '='
	| mul_op '='
;

print_stmt: PRINT '(' exp_list ')'
;

println_stmt: PRINTLN '(' exp_list ')'
;

return_stmt: RETURN
	| RETURN exp
;

if_stmt: IF simple_stmt ';' exp block
	| IF simple_stmt ';' exp block ELSE if_stmt
	| IF simple_stmt ';' exp block ELSE block
	| IF exp block
	| IF exp block ELSE if_stmt
	| IF exp block ELSE block
;

switch_stmt: SWITCH simple_stmt ';' exp '{' exp_case_clause '}'
	| SWITCH simple_stmt ';' '{' exp_case_clause '}'
	| SWITCH exp '{' exp_case_clause '}'
	| SWITCH '{' exp_case_clause '}'
;

exp_case_clause: exp_switch_case ':' stmt_list exp_case_clause
	| /* emtpy */
;
exp_switch_case: CASE exp_list
	| DEFAULT
;

for_stmt: FOR for_clause block
;

for_clause: condition
	| opt_simple_stmt ';' condition ';' opt_simple_stmt
;

condition: exp
	| /* empty */
;

opt_simple_stmt: simple_stmt
	| /* empty */
;

break_stmt: BREAK;

continue_stmt: CONTINUE;

/* 2.9 Expressions */
exp_list: exp_list ',' exp
	| exp
	| /* empty */
;

exp: unary_exp
	| exp OR exp
	| exp AND exp
	| exp EQ exp
	| exp NEQ exp
	| exp '<' exp
	| exp LEQ exp
	| exp '>' exp
	| exp GEQ exp
	| exp '+' exp
	| exp '-' exp
	| exp '|' exp
	| exp '^' exp
	| exp '*' exp
	| exp '/' exp
	| exp '%' exp
	| exp LEFTSHIFT exp
	| exp RIGHTSHIFT exp
	| exp '&' exp
	| exp BITCLEAR exp
;

unary_exp: unary_op unary_exp
	| prim_exp
;

prim_exp: operand
	| prim_exp selector
	| prim_exp index
	| func_call
	| append_exp
	| type_cast_exp
;

operand: literal
	| IDENTIFIER
	| '(' exp ')'
;

selector: '.' IDENTIFIER
;

index: '[' INTCONST ']'
;

literal: INTCONST
	| FLOATCONST
	| BOOLCONST
	| RUNECONST
	| string_lit
;

string_lit: STRINGCONST
	| RAWSTRINGCONST
;

/* OPERATORS */
unary_op: '+'         /* TODO make sure precedence works */
	| '-'
	| '!'
	| '^'
	| '*'
		{ yyerror("unsupported unary operator"); }
	| '&'
		{ yyerror("unsupported unary operator"); }
	| CHANDIRECTION
		{ yyerror("unsupported unary operator"); }
;

add_op: '+'
	| '-'
	| '|'
	| '^'
;

mul_op: '*'
	| '/'
	| '%'
	| LEFTSHIFT
	| RIGHTSHIFT
	| '&'
	| BITCLEAR
;

/* 2.9.6 Function Calls */
func_call: IDENTIFIER '(' func_call_list ')'
;

func_call_list: id_list /* TODO */
;

/* 2.9.7 Append */
append_exp: APPEND '(' IDENTIFIER ',' exp ')'

/* 2.9.8 Type casts */
/* cast to base types supported here, to aliases supported later in compiler */
type_cast_exp: basic_type "(" IDENTIFIER ")";

opt_semi:
	| ';'
;
%%
