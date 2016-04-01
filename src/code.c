#include "code.h"

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

void codeSYMBOL(SYMBOL *obj) {
	print(obj->id);
}

void codeID(ID *obj) {
	if (obj->next != NULL) {
		codeID(obj->next);
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

void codePROGRAM(PROGRAM *obj, int indentation, int pptype_flag) {
  pptype_flag_on = pptype_flag; // for flag --pptype
	codePACKAGE(obj->package, indentation);
	codeTOP_DECL(obj->top_decl, indentation);
}

void codePACKAGE(PACKAGE *obj, int indentation) {
	print("package ");
	codeID(obj->id);
	print("\n\n");
}

void codeTOP_DECL(TOP_DECL *obj, int indentation) {
	if (!obj) return;
	if (obj->next) {
		codeTOP_DECL(obj->next, indentation);
	}

	switch(obj->kind) {
		case top_varK:
			codeVAR_DECL(obj->val.varT, indentation);
			break;
		case top_typeK:
			codeTYPE_DECL(obj->val.typeT, indentation);
			break;
		case top_funcK:
			codeFUNC_DECL(obj->val.funcT, indentation);
			break;
	}
}

void codeVAR_DECL(VAR_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeVAR_DECL(obj->next, indentation);

	print("var ");
	codeID(obj->id);

	print(" ");
	codeTYPE(obj->type);

	if (obj->exp) {
		print(" = ");
		codeEXP(obj->exp);
	}
	print("\n");
}

void codeTYPE_DECL(TYPE_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeTYPE_DECL(obj->next, indentation);

	print("type ");
	codeID(obj->id);

	print(" ");
	codeTYPE(obj->type);
	print("\n");
}

void codeTYPE(TYPE *obj) {
	if(!obj) return;

	switch(obj->kind) {
		case type_refK:
			codeID(obj->val.refT);
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
				codeTYPE(obj->val.arrayT.type);
			}
			break;
		case type_sliceK:
			print("[]");
			if(obj->val.sliceT) {
				codeTYPE(obj->val.sliceT);
			}
			break;
		case type_structK:
			print("struct {");
			if(obj->val.structT) {
				codeSTRUCT_DECL(obj->val.structT);
			}
			print("}");
			return;
	}
}

void codeSTRUCT_DECL(STRUCT_DECL *obj) {
	if (!obj) return;
	if(obj->next) {
		codeSTRUCT_DECL(obj->next);
	}

	codeID(obj->id);
	if(obj->type) {
		print(" ");
		codeTYPE(obj->type);
	}
	print("; ");
}

void codeFUNC_DECL(FUNC_DECL *obj, int indentation) {
	print("func ");
	codeID(obj->id);
	codeFUNC_SIGN(obj->signature);
	print(" {\n");
	codeSTMT(obj->body, indentation+1);
	print("}\n\n");
}

void codeFUNC_SIGN(FUNC_SIGN *obj) {
	print("(");

	if(obj->arg) {
		codeFUNC_ARG(obj->arg);
	}

	print(")");
	if(obj->type) {
		print(" ");
		codeTYPE(obj->type);
	}
}

void codeFUNC_ARG(FUNC_ARG *obj) {
	if (!obj) return;
	if(obj->next) {
		codeFUNC_ARG(obj->next);
		print(", ");
	}

	if(obj->id) {
		codeID(obj->id);
	}

	print(" ");
	codeTYPE(obj->type);
}

void codeSTMT(STMT *obj, int indentation) {
	if (!obj) return;
	if(obj->next) {
		codeSTMT(obj->next, indentation);
	}


	switch(obj->kind) {
		case emptyK:
			return;
		case blockK:
			printIndentation(indentation);
			print("{\n");
			codeSTMT(obj->val.blockS, indentation + 1);
			printIndentation(indentation);
			print("}\n");
			break;
		case varK:
			printIndentation(indentation);
			codeVAR_DECL(obj->val.varS, indentation);
			break;
		case typeK:
			printIndentation(indentation);
			codeTYPE_DECL(obj->val.typeS, indentation);
			break;
		case printK:
			printIndentation(indentation);
			print("print(");
			codeEXP(obj->val.printS);
			print(")");
			if (indentation) print("\n");
			break;
		case printlnK:
			printIndentation(indentation);
			print("println(");
			codeEXP(obj->val.printlnS);
			print(")");
			if (indentation) print("\n");
			break;
		case shortvarK:
			if (indentation) printIndentation(indentation);
			codeEXP(obj->val.shortvarS.left);
			print(" := ");
			codeEXP(obj->val.shortvarS.right);
			if (indentation) print("\n");
			break;
		case returnK:
			printIndentation(indentation);
			print("return ");
			codeEXP(obj->val.returnS);
			if (indentation) print("\n");
			break;
		case ifK:
			print("\n");
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifS.pre_stmt) {
				codeSTMT(obj->val.ifS.pre_stmt, 0);
				print("; ");
			}
			codeEXP(obj->val.ifS.condition);
			print(" {\n");

			codeSTMT(obj->val.ifS.body, indentation + 1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case ifelseK:
			print("\n");
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifelseS.pre_stmt) {
				codeSTMT(obj->val.ifelseS.pre_stmt, 0);
				print("; ");
			}
			codeEXP(obj->val.ifelseS.condition);
			print(" {\n");

			codeSTMT(obj->val.ifelseS.thenpart, indentation + 1);

			printIndentation(indentation);
			print("} else {\n");

			codeSTMT(obj->val.ifelseS.elsepart, indentation+1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case switchK:
			print("\n");
			printIndentation(indentation);
			print("switch ");
			if(obj->val.switchS.pre_stmt) {
				codeSTMT(obj->val.switchS.pre_stmt, 0);
				print("; ");
			}

			if(obj->val.switchS.condition) {
				codeEXP(obj->val.switchS.condition);
			}
			print(" {\n");

			codeCASE_DECL(obj->val.switchS.body, indentation+1);

			printIndentation(indentation);
			print("}");
			if (indentation) print("\n");
			break;
		case forK:
			print("\n");
			printIndentation(indentation);
			print("for ");
			codeFOR_CLAUSE(obj->val.forS.for_clause);
			print("{\n");

			codeSTMT(obj->val.forS.body, indentation+1);

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
			codeEXP(obj->val.assignS.left);
			print(" = ");
			codeEXP(obj->val.assignS.right);
			if (indentation) print("\n");
			break;
		case expK:
			if (indentation) printIndentation(indentation);
			codeEXP(obj->val.expS);
			if (indentation) print("\n");
			break;
	}
}

void codeCASE_DECL(CASE_DECL *obj, int indentation) {
	if (!obj) return;
	if(obj->next) codeCASE_DECL(obj->next, indentation);

	printIndentation(indentation);

	switch(obj->kind) {
		case caseK:
			print("case ");
			codeEXP(obj->val.caseC.condition);
			print(":\n");
			codeSTMT(obj->val.caseC.stmt, indentation+1);
			break;
		case defaultK:
			print("default:\n");
			codeSTMT(obj->val.defaultC, indentation+1);
			break;
	}
}

void codeFOR_CLAUSE(FOR_CLAUSE *obj) {
	if(obj->init_stmt) {
		codeSTMT(obj->init_stmt, 0);
	}
	print("; ");

	if(obj->condition) {
		codeEXP(obj->condition);
	}
	print("; ");

	if(obj->post_stmt) {
		codeSTMT(obj->post_stmt, 0);
	}

	print(" ");
}

void codeEXP(EXP *obj) {
	if (!obj) return;
	if(obj->next) {
		codeEXP(obj->next);
		print(", ");
	}

  // print type before the expression
  if (pptype_flag_on) {
    print("/*");
    codeTYPE(obj->type);
    print("*/ ");
  }

	switch(obj->kind) {
		case idK:
			codeID(obj->val.idE);
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
			codeEXP(obj->val.binaryE.left);
			print(" + ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case minusK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" - ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case timesK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" * ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case divK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" / ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case modK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" % ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitandK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" & ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case andnotK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" &^ ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitorK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print("|");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case bitxork:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" ^ ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leftshiftK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" << ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case rightshiftK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" >> ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case eqK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" == ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case neqK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" != ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" < ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case leqK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" <= ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case geK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" > ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case geqK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" >= ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case orK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" || ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case andK:
			print("(");
			codeEXP(obj->val.binaryE.left);
			print(" && ");
			codeEXP(obj->val.binaryE.right);
			print(")");
			break;
		case unotK:
			print("!");
			codeEXP(obj->val.unaryE);
			break;
		case uplusK:
			print("+");
			codeEXP(obj->val.unaryE);
			break;
		case uminusK:
			print("-");
			codeEXP(obj->val.unaryE);
			break;
		case ubitnotK:
			print("^");
			codeEXP(obj->val.unaryE);
			break;

		case indexK:
			codeEXP(obj->val.indexE.exp);
			print("[");
			codeEXP(obj->val.indexE.index);
			print("]");
			break;
		case selectorK:
			codeEXP(obj->val.selectorE.exp);
			print(".");
			codeID(obj->val.selectorE.id);
			break;
		case funccallK:
			codeEXP(obj->val.funccallE.exp);
			print("(");
			if (obj->val.funccallE.args) codeEXP(obj->val.funccallE.args);
			print(")");
			break;
		case appendK:
			print("append(");
			codeID(obj->val.appendE.id);
			print(", ");
			codeEXP(obj->val.appendE.exp);
			print(")");
			break;
		case castK:
			codeTYPE(obj->val.castE.type);
			print("(");
			codeEXP(obj->val.castE.exp);
			print(")");
			break;
		case parenK:
			print("(");
			codeEXP(obj->val.parenE);
			print(")");
			break;
	}
}

