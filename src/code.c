#include "code.h"

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

void codePROGRAM(PROGRAM *obj, int indentation) {
	codeCPPheader();  // add #include and stuff like that

	/* codePACKAGE(obj->package, indentation); */ //ignore
	codeTOP_DECL(obj->top_decl, indentation);
}

void codeCPPheader() {
	print("#include <iostream>");
	NEWLINE;
	print("using namespace std;");
	NEWLINE; NEWLINE;
}

void codePACKAGE(PACKAGE *obj, int indentation) {
	print("namespace ");
	codeID(obj->id);
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

// type id [id = exp];;
void codeVAR_DECL(VAR_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeVAR_DECL(obj->next, indentation);

	// print declared type or else exp type
	obj->type ? codeTYPE(obj->type) : codeTYPE(obj->exp->type);
	SPACE;

	//ID might be a list, so go over it
	ID* tmpid = obj->id;
	EXP* tmpexp = obj->exp;
	while (tmpid) {
		print(tmpid->name);
		SPACE;
		if (tmpexp) {
			print("=");
			SPACE;
			codeEXPsingle(tmpexp);
			tmpexp = tmpexp->next;
		}
		tmpid = tmpid->next;
		if (tmpid) print(", ");
	}
	SEMICOLON;NEWLINE;
}

void codeTYPE_DECL(TYPE_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeTYPE_DECL(obj->next, indentation);

	print("typedef ");
	codeTYPE(obj->type);
	SPACE;
	codeID(obj->id);
	SEMICOLON;
	NEWLINE;
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
			print("double");  //double is 64-bit float in C++
			break;
		case type_boolK:
			print("bool");
			break;
		case type_runeK:
			print("char");
			break;
		case type_stringK:
			print("std::string");  // std::string
			return;
		case type_arrayK:
			if(obj->val.arrayT.type) {
				codeTYPE(obj->val.arrayT.type);
			}
			//TODO name goes here
			print("[");
			printInteger(obj->val.arrayT.size);
			print("]");
			break;
		case type_sliceK: //TODO
			print("[]");
			if(obj->val.sliceT) {
				codeTYPE(obj->val.sliceT);
			}
			break;
		case type_structK:
			print("struct { ");
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

	if(obj->type) {
		codeTYPE(obj->type);
	}
	SPACE;
	codeID(obj->id);
	SEMICOLON;
}

void codeFUNC_DECL(FUNC_DECL *obj, int indentation) {
	if (strcmp(obj->id->name, "main") == 0) {
		print("int main() ");
		print(" {\n");
		codeSTMT(obj->body, indentation+1, 0);
		print("\treturn 0;\n");
		print("}\n\n");
	} else {
		codeFUNC_SIGN(obj->signature, obj->id);
		print(" {\n");
		codeSTMT(obj->body, indentation+1, 0);
		print("}\n\n");
	}
}

void codeFUNC_SIGN(FUNC_SIGN *obj, ID *id) {
	//return type
	obj->type ? codeTYPE(obj->type) : print("void");
	SPACE;

	//name
	codeID(id);

	print("(");

	if(obj->arg) {
		codeFUNC_ARG(obj->arg);
	}

	print(")");
}

void codeFUNC_ARG(FUNC_ARG *obj) {
	if (!obj) return;
	if(obj->next) {
		codeFUNC_ARG(obj->next);
		print(", ");
	}

	if(obj->id) {
		ID* tmpid = obj->id;
		while (tmpid) {
			codeTYPE(obj->type);
			SPACE;
			print(tmpid->name);
			tmpid = tmpid->next;
			if (tmpid) print(", ");
		}
	}
}

void codeSTMT(STMT *obj, int indentation, int isInsideLoop) {
	if (!obj) return;
	if(obj->next) {
		codeSTMT(obj->next, indentation, isInsideLoop);
	}


	switch(obj->kind) {
		case emptyK:
			return;
		case blockK:
			printIndentation(indentation);
			print("{\n");
			codeSTMT(obj->val.blockS, indentation + 1, isInsideLoop);
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
			SEMICOLON;NEWLINE;
			break;
		case printK:
			codeSTMTprint(obj->val.printS, indentation);
			break;
		case printlnK:
			codeSTMTprintln(obj->val.printlnS, indentation);
			printIndentation(indentation);
			print("cout << \"\\n\";");
			NEWLINE;
			break;
		case shortvarK: //TODO
			if (indentation) printIndentation(indentation);
			codeEXPsingle(obj->val.shortvarS.left);
			print(" := ");
			codeEXPsingle(obj->val.shortvarS.right);
			if (indentation) NEWLINE;
			break;
		case returnK:
			printIndentation(indentation);
			print("return ");
			codeEXPsingle(obj->val.returnS);
			SEMICOLON;
			if (indentation) NEWLINE;
			break;
		case ifK:
			NEWLINE;
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifS.pre_stmt) {
				codeSTMT(obj->val.ifS.pre_stmt, 0, 0);
				print("; ");
			}
			codeEXPsingle(obj->val.ifS.condition);
			print(" {\n");

			codeSTMT(obj->val.ifS.body, indentation + 1, isInsideLoop);

			printIndentation(indentation);
			print("}");
			if (indentation) NEWLINE;
			break;
		case ifelseK:
			NEWLINE;
			printIndentation(indentation);
			print("if ");
			if(obj->val.ifelseS.pre_stmt) {
				codeSTMT(obj->val.ifelseS.pre_stmt, 0, 0);
				print("; ");
			}
			codeEXPsingle(obj->val.ifelseS.condition);
			print(" {\n");

			codeSTMT(obj->val.ifelseS.thenpart, indentation + 1, isInsideLoop);

			printIndentation(indentation);
			print("} else {\n");

			codeSTMT(obj->val.ifelseS.elsepart, indentation + 1, isInsideLoop);

			printIndentation(indentation);
			print("}");
			if (indentation) NEWLINE;
			break;
		case switchK: // TODO: fix the use of breaks
			NEWLINE
			printIndentation(indentation);
			print("{//switch");
			indentation += 1;
			if(obj->val.switchS.pre_stmt) {
				NEWLINE;
				printIndentation(indentation);
				codeSTMT(obj->val.switchS.pre_stmt, 0, 0);
				print("; ");
			}
			if(obj->val.switchS.condition) {
				NEWLINE;
				printIndentation(indentation);
				print("if (");
				codeEXPsingle(obj->val.switchS.condition);
				print(") {\n");
			}

			codeCASE_DECL(obj->val.switchS.body, indentation+1);

			printIndentation(indentation);
			print("}");//close condition
			NEWLINE
			printIndentation(indentation-1);
			print("}");
			if (indentation) NEWLINE;
			break;
		case forK:
			NEWLINE;
			printIndentation(indentation);
			codeFOR_CLAUSE(obj->val.forS.for_clause);
			print("{\n");

			codeSTMT(obj->val.forS.body, indentation+1, 1);

			printIndentation(indentation);
			print("}");
			if (indentation) NEWLINE;
			break;
		case breakK:
			if (!indentation)
			printIndentation(indentation);
			print("break");
			SEMICOLON;NEWLINE;
			if (indentation) NEWLINE;
			break;
		case continueK:
			if (indentation) printIndentation(indentation);
			print("continue;");
			if (indentation) NEWLINE;
			break;
		case assignK:
			codeSTMTassign(obj->val.assignS.left, obj->val.assignS.right, indentation);
			break;
		case expK:
			if (indentation) printIndentation(indentation);
			codeEXPsingle(obj->val.expS);
			if (indentation) NEWLINE;
			break;
	}
}

void codeCASE_DECL(CASE_DECL *obj, int indentation) {
	if (!obj) return;
	if(obj->next) codeCASE_DECL(obj->next, indentation);

	printIndentation(indentation);

	switch(obj->kind) {
		case caseK:
			print("if(");
			codeEXPsingle(obj->val.caseC.condition);
			print("){");
			NEWLINE;
			codeSTMT(obj->val.caseC.stmt, indentation+1, 1);
			NEWLINE;
			printIndentation(indentation);
			print("}");
			NEWLINE;
			break;
		case defaultK:
			print("else {");
			NEWLINE;
			codeSTMT(obj->val.defaultC, indentation+1, 1);
			NEWLINE;
			printIndentation(indentation);
			print("}");
			NEWLINE;
			break;
	}
}

void codeFOR_CLAUSE(FOR_CLAUSE *obj) {
	//3-part
	if(obj->init_stmt && obj->condition && obj->post_stmt) {
		print("for(");
		codeSTMT(obj->init_stmt,0, 0);
		codeEXPsingle(obj->condition);
		SEMICOLON;
		codeSTMT(obj->post_stmt, 0, 0);
		print(") ");
		return;
	}

	//while loop
	if (obj->condition) {
		print("while");
		codeEXPsingle(obj->condition);
		return;
	}

	//infinite
	print("while(0) ");
}

void codeSTMTprint(EXP *exp, int indentation) {
	if (!exp) return;
	if (exp->next) {
		codeSTMTprint(exp->next, indentation);
	}

	printIndentation(indentation);
	print("cout << ");
	codeEXPsingle(exp);
	SEMICOLON;
	NEWLINE;
}

void codeSTMTprintln(EXP *exp, int indentation) {
	if (!exp) return;
	if (exp->next) {
		codeSTMTprintln(exp->next, indentation);
	}

	printIndentation(indentation);
	print("cout << ");
	codeEXPsingle(exp);
	SEMICOLON;
	NEWLINE;

	printIndentation(indentation);
	print("cout << \" \";");
	printIndentation(indentation);
	NEWLINE;
}

void codeSTMTassign(EXP *left, EXP *right, int indentation) {
	if (!left || !right ) return;
	if (left->next && right->next) {
		codeSTMTassign(left->next, right->next, indentation);
	}

	if (indentation) printIndentation(indentation);
	codeEXPsingle(left);
	print(" = ");
	codeEXPsingle(right);
	SEMICOLON;
	if (indentation) NEWLINE;
}

void codeEXP(EXP *obj) {
	if (!obj) return;
	if(obj->next) {
		codeEXP(obj->next);
		print(", ");
	}

	codeEXPsingle(obj);
}

void codeEXPsingle(EXP *obj) {
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

