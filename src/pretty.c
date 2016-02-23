#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pretty.h"
#include "tree.h"

extern FILE *prettyFile;

void print(char *text) {
	/* fprintf(prettyFile, "%s", text); */
	printf("%s", text);
}

void printInteger(int number) {
	/* fprintf(prettyFile, "%d", number); */
	printf("%d", number);
}

void printFloat(float number) {
	/* fprintf(prettyFile, "%f", number); */
	printf("%f",number);
}

void printIndentation(int i) {
	int j = 0;

	print("\n");

	for( ; j < i; j++) {
		print("\t");
	}
}

void prettyPROGRAM(PROGRAM *obj, int indentation) {
	prettyPACKAGE(obj->package, indentation);
	prettyTOP_DECL(obj->top_decl, indentation);
}

void prettyPACKAGE(PACKAGE *obj, int indentation) {
	print("package ");
	print(obj->name);
}

void prettyTOP_DECL(TOP_DECL *obj, int indentation) {
	if (obj->next) {
		prettyTOP_DECL(obj->next, indentation);
	}

	printIndentation(indentation);

	switch(obj->kind) {
		case top_varK:
			prettyVAR_DECL(obj->val.varT, indentation);
			break;
		case top_typeK:
			if(obj->val.typeT) {
				prettyTYPE_DECL(obj->val.typeT, indentation);
			}
			break;
		case top_funcK:
			prettyFUNC_DECL(obj->val.funcT, indentation);
			break;
	}
}

void prettyVAR_DECL(VAR_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) prettyVAR_DECL(obj->next, indentation);

	printIndentation(indentation);
	print("var ");
	prettyID(obj->id);
	print(" ");

	prettyTYPE(obj->type);

	if (obj->exp) {
		print(" = ");
		prettyEXP(obj->exp);
	}
}

void prettyTYPE_DECL(TYPE_DECL *obj, int indentation) {
	if(obj->next) prettyTYPE_DECL(obj->next, indentation);

	printIndentation(indentation);
	print("type ");
	print(obj->name);
	print(" ");

	prettyTYPE(obj->type);
}

void prettyID(ID *obj) {
	if(obj->next) {
		prettyID(obj->next);
		print(", ");
	}

	print(obj->name);
}

void prettyFUNC_DECL(FUNC_DECL *obj, int indentation) {
	printIndentation(indentation);

	print("func ");
	print(obj->name);
	prettyFUNC_SIGN(obj->signature);
	print("{");
	prettySTMT(obj->body, indentation+1);
	print("}");
}

void prettyTYPE(TYPE *obj) {
	if(!obj) return;

	switch(obj->kind) {
		case type_refK:
			print(obj->name);
			return;
		case type_intK:
			print("int");
			return;
		case type_floatK:
			print("float64");
			break;
		case type_boolK:
			print("bool");
			break;
		case type_runeK:
			print("rune");
			break;
		case type_stringK:
			print("string");
			return;
		case type_arrayK:
			print("[");
			printInteger(obj->val.arrayT.size);
			print("] ");
			if(obj->val.arrayT.type) {
				prettyTYPE(obj->val.arrayT.type);
			}
			break;
		case type_sliceK:
			print("[] ");
			if(obj->val.sliceT.type) {
				prettyTYPE(obj->val.sliceT.type);
			}
			break;
		case type_structK:
			print("struct {");
			if(obj->val.structT.struct_decl) {
				prettySTRUCT_DECL(obj->val.structT.struct_decl);
			}
			print("}");
			return;
	}
}

void prettyEXP(EXP *obj) {
	if(obj->next) {
		prettyEXP(obj->next);
		print(", ");
	}

	switch(obj->kind) {
		case idK:
			prettySYMBOL(obj->val.idE.sym);
			break;
		case intconstK:
			printInteger(obj->val.intconstE);
			break;
		case floatconstK:
			printFloat(obj->val.floatconstE);
			break;
		case stringconstK:
			print(obj->val.stringconstE);
			break;
		case rawstringconstK:
			print(obj->val.rawstringconstE);
			break;
		case plusK:
			prettyEXP(obj->val.binaryE.left);
			print(" + ");
			prettyEXP(obj->val.binaryE.right);
			break;
		case minusK:
			prettyEXP(obj->val.binaryE.left);
			print(" - ");
			prettyEXP(obj->val.binaryE.right);
			break;
		case timesK:
			prettyEXP(obj->val.binaryE.left);
			print(" * ");
			prettyEXP(obj->val.binaryE.right);
			break;
		case divK:
			prettyEXP(obj->val.binaryE.left);
			print(" / ");
			prettyEXP(obj->val.binaryE.right);
			break;
		case modK:
			print(" % ");
			break;
		case bitandK:
			print(" & ");
			break;
		case andnotK:
			print(" &^ ");
			break;
		case bitorK:
			print("|");
			break;
		case bitxork:
			print(" ^ ");
			break;
		case leftshiftK:
			print(" << ");
			break;
		case rightshiftK:
			print(" >> ");
			break;
		case eqK:
			print(" == ");
			break;
		case neqK:
			print(" != ");
			break;
		case leK:
			print(" < ");
			break;
		case leqK:
			print(" <= ");
			break;
		case geK:
			print(" > ");
			break;
		case geqK:
			print(" >= ");
			break;
		case orK:
			print(" || ");
			break;
		case andK:
			print(" && ");
			break;
		case unotK:
			print("!");
			break;
		case uplusK:
			print("+");
			break;
		case uminusK:
			print("-");
			prettyEXP(obj->val.unaryE);
			break;
		case ubitnotK:
			print("^");
			break;
	}
}

void prettySTRUCT_DECL(STRUCT_DECL *obj) {
	if(obj->next) {
		prettySTRUCT_DECL(obj->next);
		print(", ");
	}

	prettyID(obj->id);
	if(obj->type) {
		print(" ");
		prettyTYPE(obj->type);
	}
}

void prettyFUNC_SIGN(FUNC_SIGN *obj) {
	print("(");

	if(obj->arg) {
		prettyFUNC_ARG(obj->arg);
	}

	print(") ");
	if(obj->type) {
		prettyTYPE(obj->type);
	}
}

void prettySTMT(STMT *obj, int indentation) {
	if(obj->next) {
		prettySTMT(obj->next, indentation);
		print("\n");
	}


	switch(obj->kind) {
		case emptyK:
			break;
		case typeK:
			if (indentation) printIndentation(indentation);
			prettyTYPE_DECL(obj->val.typeS, indentation);
			break;
		case printK:
			if (indentation) printIndentation(indentation);
			print("print(");
			prettyEXP(obj->val.printS);
			print(")");
			break;
		case printlnK:
			if (indentation) printIndentation(indentation);
			print("println(");
			prettyEXP(obj->val.printlnS);
			break;
		case shortvarK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.shortvarS.left);
			print(" := ");
			prettyEXP(obj->val.shortvarS.right);
			break;
		case returnK:
			if (indentation) printIndentation(indentation);
			print("return ");
			prettyEXP(obj->val.returnS);
			break;
		case ifK:
			if (indentation) printIndentation(indentation);
			print("if ");
			if(obj->val.ifS.pre_stmt) {
				prettySTMT(obj->val.ifS.pre_stmt, 0);
				print("; ");
			}
			prettyEXP(obj->val.ifS.condition);
			print(" {");

			prettySTMT(obj->val.ifS.body, indentation + 1);

			printIndentation(indentation);
			print("}");
			break;
		case ifelseK:
			if (indentation) printIndentation(indentation);
			print("if ");
			if(obj->val.ifelseS.pre_stmt) {
				prettySTMT(obj->val.ifelseS.pre_stmt, 0);
				print("; ");
			}
			prettyEXP(obj->val.ifelseS.condition);
			print(" {");

			prettySTMT(obj->val.ifelseS.thenpart, indentation + 1);

			printIndentation(indentation);
			print("} else {");

			prettySTMT(obj->val.ifelseS.elsepart, indentation+1);

			printIndentation(indentation);
			print("}");
			break;
		case switchK:
			if (indentation) printIndentation(indentation);
			print("switch ");
			if(obj->val.switchS.pre_stmt) {
				prettySTMT(obj->val.switchS.pre_stmt, 0);
				print("; ");
			}

			if(obj->val.switchS.condition) {
				prettyEXP(obj->val.switchS.condition);
			}
			print(" {");

			if(obj->val.switchS.body) {
				prettyCASE_DECL(obj->val.switchS.body, indentation+1);
			}

			printIndentation(indentation);
			print("}");
			break;
		case forK:
			if (indentation) printIndentation(indentation);
			prettyFOR_CLAUSE(obj->val.forS.for_clause);
			print("{\n");

			prettySTMT(obj->val.forS.body, indentation+1);

			printIndentation(indentation);
			print("}");
			break;
		case breakK:
			print("break;\n");
			break;
		case continueK:
			print("continue\n");
			break;
		case assignK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.assignS.left);
			print(" = ");
			prettyEXP(obj->val.assignS.right);
			break;
		case expK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.expS);
			break;
	}
}

void prettySYMBOL(SYMBOL *obj) {
	print(obj->id);

	if(obj->next) {
		print(", ");
		prettySYMBOL(obj->next);
	}
}

void prettyFUNC_ARG(FUNC_ARG *obj) {
	if(obj->next) {
		prettyFUNC_ARG(obj->next);
		print(", ");
	}

	if(obj->id) {
		prettyID(obj->id);
	}

	print(" ");
	prettyTYPE(obj->type);
}

void prettyCASE_DECL(CASE_DECL *obj, int indentation) {
	if(obj->next) prettyCASE_DECL(obj->next, indentation);

	printIndentation(indentation);

	switch(obj->kind) {
		case caseK:
			print("case ");
			prettyEXP(obj->val.caseC.condition);
			print(":\n");
			prettySTMT(obj->val.caseC.stmt, indentation+1);
			break;
		case defaultK:
			print("default:\n");
			prettySTMT(obj->val.defaultC, indentation+1);
			break;
	}
}

void prettyFOR_CLAUSE(FOR_CLAUSE *obj) {
	print("for ");
	if(obj->init_stmt) {
		prettySTMT(obj->init_stmt, 0);
	}
	print("; ");

	if(obj->condition) {
		prettyEXP(obj->condition);
	}
	print("; ");

	if(obj->post_stmt) {
		prettySTMT(obj->post_stmt, 0);
	}

	print(" ");
}

