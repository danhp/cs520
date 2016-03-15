#include "pretty.h"

extern FILE *prettyFile;

void print(char *text) {
	printf("%s", text);
}

void printInteger(int number) {
	printf("%d", number);
}

void printFloat(float number) {
	printf("%f",number);
}

void printRune(char c) {
	print("'");
	switch (c) {
		case '\a':
			printf("\\a");
			break;
		case '\b':
			printf("\\b");
			break;
		case '\f':
			printf("\\f");
			break;
		case '\n':
			printf("\\n");
			break;
		case '\r':
			printf("\\r");
			break;
		case '\t':
			printf("\\t");
			break;
		case '\v':
			printf("\\v");
			break;
		case '\\':
			printf("\\\\");
			break;
		case '\'':
			printf("\\'");
			break;
		default:
			printf("%c", c);
			break;
	}
	print("'");
}

void prettySYMBOL(SYMBOL *obj) {
	print(obj->id);
}

void prettyID(ID *obj) {
	if (obj->next != NULL) {
		prettyID(obj->next);
		print(", ");
	}

	print(obj->name);
}

void printIndentation(int i) {
	int j = 0;

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
	prettyID(obj->id);
	print("\n\n");
}

void prettyTOP_DECL(TOP_DECL *obj, int indentation) {
	if (obj->next) {
		prettyTOP_DECL(obj->next, indentation);
	}

	switch(obj->kind) {
		case top_varK:
			prettyVAR_DECL(obj->val.varT, indentation);
			break;
		case top_typeK:
			prettyTYPE_DECL(obj->val.typeT, indentation);
			break;
		case top_funcK:
			prettyFUNC_DECL(obj->val.funcT, indentation);
			break;
	}
}

void prettyVAR_DECL(VAR_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) prettyVAR_DECL(obj->next, indentation);

	print("var ");
	prettyID(obj->id);

	print(" ");
	prettyTYPE(obj->type);

	if (obj->exp) {
		print(" = ");
		prettyEXP(obj->exp);
	}
	print("\n");
}

void prettyTYPE_DECL(TYPE_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) prettyTYPE_DECL(obj->next, indentation);

	print("type ");
	prettyID(obj->id);

	print(" ");
	prettyTYPE(obj->type);
	print("\n");
}

void prettyTYPE(TYPE *obj) {
	if(!obj) return;

	switch(obj->kind) {
		case type_refK:
			prettyID(obj->val.refT);
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
			print("]");
			if(obj->val.arrayT.type) {
				prettyTYPE(obj->val.arrayT.type);
			}
			break;
		case type_sliceK:
			print("[]");
			if(obj->val.sliceT) {
				prettyTYPE(obj->val.sliceT);
			}
			break;
		case type_structK:
			print("struct {");
			if(obj->val.structT) {
				prettySTRUCT_DECL(obj->val.structT);
			}
			print("}");
			return;
	}
}

void prettySTRUCT_DECL(STRUCT_DECL *obj) {
	if (!obj) return;
	if(obj->next) {
		prettySTRUCT_DECL(obj->next);
	}

	prettyID(obj->id);
	if(obj->type) {
		print(" ");
		prettyTYPE(obj->type);
	}
	print("; ");
}

void prettyFUNC_DECL(FUNC_DECL *obj, int indentation) {
	print("func ");
	prettyID(obj->id);
	prettyFUNC_SIGN(obj->signature);
	print(" {\n");
	prettySTMT(obj->body, indentation+1);
	print("}\n\n");
}

void prettyFUNC_SIGN(FUNC_SIGN *obj) {
	print("(");

	if(obj->arg) {
		prettyFUNC_ARG(obj->arg);
	}

	print(")");
	if(obj->type) {
		print(" ");
		prettyTYPE(obj->type);
	}
}

void prettyFUNC_ARG(FUNC_ARG *obj) {
	if (!obj) return;
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

void prettySTMT(STMT *obj, int indentation) {
	if (!obj) return;
	if(obj->next) {
		prettySTMT(obj->next, indentation);
	}


	switch(obj->kind) {
		case emptyK:
			return;
		case blockK:
			printIndentation(indentation);
			print("{\n");
			prettySTMT(obj->val.blockS, indentation + 1);
			printIndentation(indentation);
			print("}\n");
			break;
		case varK:
			printIndentation(indentation);
			prettyVAR_DECL(obj->val.varS, indentation);
			break;
		case typeK:
			printIndentation(indentation);
			prettyTYPE_DECL(obj->val.typeS, indentation);
			break;
		case printK:
			printIndentation(indentation);
			print("print(");
			prettyEXP(obj->val.printS);
			print(")");
			if (indentation) print("\n");
			break;
		case printlnK:
			printIndentation(indentation);
			print("println(");
			prettyEXP(obj->val.printlnS);
			print(")");
			if (indentation) print("\n");
			break;
		case shortvarK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.shortvarS.left);
			print(" := ");
			prettyEXP(obj->val.shortvarS.right);
			if (indentation) print("\n");
			break;
		case returnK:
			printIndentation(indentation);
			print("return ");
			prettyEXP(obj->val.returnS);
			if (indentation) print("\n");
			break;
		case ifK:
			print("\n");
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifS.pre_stmt) {
				prettySTMT(obj->val.ifS.pre_stmt, 0);
				print("; ");
			}
			prettyEXP(obj->val.ifS.condition);
			print(" {\n");

			prettySTMT(obj->val.ifS.body, indentation + 1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case ifelseK:
			print("\n");
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifelseS.pre_stmt) {
				prettySTMT(obj->val.ifelseS.pre_stmt, 0);
				print("; ");
			}
			prettyEXP(obj->val.ifelseS.condition);
			print(" {\n");

			prettySTMT(obj->val.ifelseS.thenpart, indentation + 1);

			printIndentation(indentation);
			print("} else {\n");

			prettySTMT(obj->val.ifelseS.elsepart, indentation+1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case switchK:
			print("\n");
			printIndentation(indentation);
			print("switch ");
			if(obj->val.switchS.pre_stmt) {
				prettySTMT(obj->val.switchS.pre_stmt, 0);
				print("; ");
			}

			if(obj->val.switchS.condition) {
				prettyEXP(obj->val.switchS.condition);
			}
			print(" {\n");

			prettyCASE_DECL(obj->val.switchS.body, indentation+1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case forK:
			print("\n");
			printIndentation(indentation);
			print("for ");
			prettyFOR_CLAUSE(obj->val.forS.for_clause);
			print("{\n");

			prettySTMT(obj->val.forS.body, indentation+1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case breakK:
			printIndentation(indentation);
			print("break;");
			if (indentation) print("\n");
			break;
		case continueK:
			if (indentation) printIndentation(indentation);
			print("continue");
			if (indentation) print("\n");
			break;
		case assignK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.assignS.left);
			print(" = ");
			prettyEXP(obj->val.assignS.right);
			if (indentation) print("\n");
			break;
		case expK:
			if (indentation) printIndentation(indentation);
			prettyEXP(obj->val.expS);
			if (indentation) print("\n");
			break;
	}
}

void prettyCASE_DECL(CASE_DECL *obj, int indentation) {
	if (!obj) return;
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

void prettyEXP(EXP *obj) {
	if (!obj) return;
	if(obj->next) {
		prettyEXP(obj->next);
		print(", ");
	}

	switch(obj->kind) {
		case idK:
			prettyID(obj->val.idE);
			break;
		case intconstK:
			printInteger(obj->val.intconstE);
			break;
		case floatconstK:
			printFloat(obj->val.floatconstE);
			break;
		case runeconstK:
			printRune(obj->val.runeconstE);
			break;
		case stringconstK:
			print("\"");
			print(obj->val.stringconstE);
			print("\"");
			break;
		case rawstringconstK:
			print("\"");
			print(obj->val.rawstringconstE);
			print("\"");
			break;
		case plusK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" + ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case minusK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" - ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case timesK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" * ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case divK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" / ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case modK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" % ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitandK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" & ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case andnotK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" &^ ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitorK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print("|");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitxork:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" ^ ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leftshiftK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" << ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case rightshiftK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" >> ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case eqK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" == ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case neqK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" != ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" < ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leqK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" <= ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case geK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" > ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case geqK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" >= ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case orK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" || ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case andK:
			print("(");
			prettyEXP(obj->val.binaryE.left);
			print(" && ");
			prettyEXP(obj->val.binaryE.right);
			print(")");
			break;
		case unotK:
			print("!");
			prettyEXP(obj->val.unaryE);
			break;
		case uplusK:
			print("+");
			prettyEXP(obj->val.unaryE);
			break;
		case uminusK:
			print("-");
			prettyEXP(obj->val.unaryE);
			break;
		case ubitnotK:
			print("^");
			prettyEXP(obj->val.unaryE);
			break;

		case indexK:
			prettyEXP(obj->val.indexE.exp);
			print("[");
			prettyEXP(obj->val.indexE.index);
			print("]");
			break;
		case selectorK:
			prettyEXP(obj->val.selectorE.exp);
			print(".");
			prettyID(obj->val.selectorE.id);
			break;
		case funccallK:
			prettyEXP(obj->val.funccallE.exp);
			print("(");
			if (obj->val.funccallE.args) prettyEXP(obj->val.funccallE.args);
			print(")");
			break;
		case appendK:
			print("append(");
			prettyID(obj->val.appendE.id);
			print(", ");
			prettyEXP(obj->val.appendE.exp);
			print(")");
			break;
		case castK:
			prettyTYPE(obj->val.castE.type);
			print("(");
			prettyEXP(obj->val.castE.exp);
			print(")");
			break;
		case parenK:
			print("(");
			prettyEXP(obj->val.parenE);
			print(")");
			break;
	}
}

