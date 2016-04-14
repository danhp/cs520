#include "code.h"

void codeID(ID *obj) {
	if (obj->next != NULL) {
		codeID(obj->next);
		print(", ");
	}
	codeIDsingle(obj);
}

void codeIDsingle(ID *id) {
	/* Don't tag golang keywords */
	if (strcmp(id->name, "true") && strcmp(id->name, "false")) {
		print("_");
	}
	print(id->name);
}

void codePROGRAM(PROGRAM *obj, int indentation) {
	codeCPPheader();  // add #include and stuff like that

	/* codePACKAGE(obj->package, indentation); */ //ignore
	codeTOP_DECL(obj->top_decl, indentation);
}

void codeCPPheader() {
	print("#include <iostream>\n");
	print("#include <string>\n");
	print("#include <vector>\n");
	print("#include <array>\n");
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
			SEMICOLON;NEWLINE;
			break;
		case top_typeK:
			codeTYPE_DECL(obj->val.typeT, indentation);
			SEMICOLON;NEWLINE;
			break;
		case top_funcK:
			codeFUNC_DECL(obj->val.funcT, indentation);
			break;
	}
}

// type id [id = exp]
void codeVAR_DECL(VAR_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeVAR_DECL(obj->next, indentation);

	// print declared type or else exp type
	obj->type ? codeTYPE(obj->type) : codeTYPE(obj->exp->type);
	SPACE;

	// ID might be a list, so go over it
	ID* id = obj->id;
	EXP* exp = obj->exp;

	while (id) {
		if (strcmp(id->name, "_") == 0) {
			id = id->next;
			exp = exp->next;
		}

		if (id) {
			if (id->next && strcmp(id->next->name, "_") != 0) {
				print(", ");
			}
			codeIDsingle(id);
			if (exp) {
				print(" = ");
				codeEXPsingle(exp);
				exp = exp->next;
			} else {
				codeEXPdefaultValueWithEqual(id->symbol->val.type);
			}
			id = id->next;
		}
	}
}

void codeTYPE_DECL(TYPE_DECL *obj, int indentation) {
	if(!obj) return;
	if(obj->next) codeTYPE_DECL(obj->next, indentation);

	if (obj->type->kind != type_structK) {
		print("typedef ");
		codeTYPE(obj->type);
		SPACE;
		codeID(obj->id);
	} else {
		print("struct ");
		codeID(obj->id);
		print(" {");
		if(obj->type->val.structT) {
			codeSTRUCT_DECL(obj->type->val.structT);
		}
		print("}");
	}
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
			print("string");  // std::string
			return;
		case type_arrayK:
			print("array<");
			codeTYPE(obj->val.arrayT.type);
			print(", ");
			printInteger(obj->val.arrayT.size);
			print(">");
			break;
		case type_sliceK:
			print("vector<");
			if(obj->val.sliceT) {
				codeTYPE(obj->val.sliceT);
			}
			print(">");
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

	SPACE;
	if(obj->type) {
		codeTYPE(obj->type);
	}
	SPACE;
	codeSTRUCT_DECLsingle(obj->id, obj->type);
	SEMICOLON;
}

void codeSTRUCT_DECLsingle(ID *id, TYPE *type) {
	if (!id) return;
	if (id->next) {
		codeSTRUCT_DECLsingle(id->next, type);
		print(", ");
	}

	codeIDsingle(id);
	codeEXPdefaultValueWithEqual(type);
}

void codeFUNC_DECL(FUNC_DECL *obj, int indentation) {
	if (strcmp(obj->id->name, "main") == 0) {
		print("int main() {\n");
		print("\ttry {\n");

		codeSTMT(obj->body, indentation+2);

		print("\t} catch (const std::out_of_range& e) {\n");
		print("\t\tstd::cout << \"\\nIndex out of bounds: \" << e.what() << '\\n';\n");
		print("\t}\n");
		print("\treturn 0;\n");
		print("}\n\n");
	} else {
		codeFUNC_SIGN(obj->signature, obj->id);
		print(" {\n");
		codeSTMT(obj->body, indentation + 1);
		print("}\n\n");
	}
}

void codeFUNC_SIGN(FUNC_SIGN *obj, ID *id) {
	//return type
	obj->type ? codeTYPE(obj->type) : print("void");
	SPACE;

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
			codeIDsingle(tmpid);
			tmpid = tmpid->next;
			if (tmpid) print(", ");
		}
	}
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
			if (indentation) {
				SEMICOLON;NEWLINE;
			}
			break;

		case typeK:
			printIndentation(indentation);
			codeTYPE_DECL(obj->val.typeS, indentation);
			if (indentation) {
				SEMICOLON;NEWLINE;
			}
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

		case shortvarK:
			/* Dummy expression */
			if (indentation) printIndentation(0);

			EXP *left = obj->val.shortvarS.left;
			EXP *right = obj->val.shortvarS.right;

			// Traverse the list
			while (left) {
				if (strcmp(left->val.idE->name, "_") == 0) {
					left = left->next;
					right = right->next;
				}
				if (left && right) {
					if (indentation) { printIndentation(indentation); }
					if (left->val.idE->symbol->shortVarNew) {
						left->type ? codeTYPE(left->type) : codeTYPE(right->type);
						SPACE;
					}
					codeEXPsingle(left);
					print(" = ");
					codeEXPsingle(right);
					SEMICOLON;
					if (indentation) { NEWLINE; }

					left = left->next;
					right = right->next;
				}
			}
			break;

		case returnK:
			printIndentation(indentation);
			print("return ");
			codeEXPsingle(obj->val.returnS);
			SEMICOLON; NEWLINE;
			break;

		case ifK:
			NEWLINE;
			if (obj->val.ifS.pre_stmt) {
				printIndentation(indentation);
				print("{ // new scope for if prestatement\n");
				indentation++;
				codeSTMT(obj->val.ifS.pre_stmt, indentation);
			}

			printIndentation(indentation);
			print("if (");
			codeEXPsingle(obj->val.ifS.condition);
			print(") {\n");
			codeSTMT(obj->val.ifS.body, indentation + 1);
			printIndentation(indentation);
			print("}\n");

			if (obj->val.ifS.pre_stmt) {
				printIndentation(indentation - 1);
				print("} // close scope for if prestatement\n");
			}
			break;

		case ifelseK:
			NEWLINE;
			if (obj->val.ifelseS.pre_stmt) {
				printIndentation(indentation);
				print("{ // new scope for ifelse prestatement\n");
				indentation++;
				codeSTMT(obj->val.ifelseS.pre_stmt, indentation);
			}

			printIndentation(indentation);
			print("if (");
			codeEXPsingle(obj->val.ifelseS.condition);
			print(") {\n");
			codeSTMT(obj->val.ifelseS.thenpart, indentation + 1);
			printIndentation(indentation);
			print("} else {\n");
			codeSTMT(obj->val.ifelseS.elsepart, indentation + 1);
			printIndentation(indentation);
			print("}\n");

			if (obj->val.ifelseS.pre_stmt) {
				printIndentation(indentation - 1);
				print("} // close scope for ifelse prestatement\n");
			}
			break;

		case switchK:
			NEWLINE;
			printIndentation(indentation);
			print("switch (true) {\n");
			printIndentation(indentation);
			print("default: \n");

			if(obj->val.switchS.pre_stmt) {
				codeSTMT(obj->val.switchS.pre_stmt, indentation + 1);
			}

			codeCASE_DECLcase(obj->val.switchS.body, obj->val.switchS.condition, indentation+1);
			codeCASE_DECLdefault(obj->val.switchS.body, indentation + 1);

			printIndentation(indentation);
			print("}\n");
			break;

		case forK:
			NEWLINE;
			printIndentation(indentation);
			codeFOR_CLAUSE(obj->val.forS.for_clause);
			print("{\n");
			codeSTMT(obj->val.forS.body, indentation + 1);
			printIndentation(indentation);
			print("}\n");
			break;

		case breakK:
			if (indentation) printIndentation(indentation);
			print("break;\n");
			break;

		case continueK:
			if (indentation) printIndentation(indentation);
			print("continue;\n");
			break;

		case assignK:
			codeSTMTassign(obj->val.assignS.left, obj->val.assignS.right, indentation);
			if (indentation) {
				SEMICOLON; NEWLINE;
			}
			break;

		case expK:
			if (indentation) printIndentation(indentation);
			codeEXPsingle(obj->val.expS);
			if (indentation) {
				SEMICOLON; NEWLINE;
			}
			break;
	}
}

void codeCASE_DECLcase(CASE_DECL *obj, EXP *condition, int indentation) {
	if (!obj) return;
	if(obj->next) codeCASE_DECLcase(obj->next, condition, indentation);


	switch(obj->kind) {
		case caseK:
			printIndentation(indentation);
			if (obj->next) {
				print("else if (");
			} else {
				print("if (");
			}
			codeEXPsingle(obj->val.caseC.condition);
			if (condition) {
				print(" == ");
				codeEXPsingle(condition);
			}
			print(") {\n");
			codeSTMT(obj->val.caseC.stmt, indentation + 1);
			printIndentation(indentation);
			print("}\n");
			break;
		case defaultK:
			// Do nothing as we want to stick these at the end.
			break;
	}
}

// Defaults are all stuck at the end
void codeCASE_DECLdefault(CASE_DECL *decl, int indentation) {
	if (!decl) return;
	if (decl->next) codeCASE_DECLdefault(decl->next, indentation);

	// Determine if this is a lone default
	int caseDeclFound = 0;
	CASE_DECL *d = decl;
	while(d && !caseDeclFound) {
		if (d->kind == caseK) {
			caseDeclFound = 1;
			break;
		}
		d = d->next;
	}

	if (decl->kind == defaultK) {
		if (caseDeclFound) {
			printIndentation(indentation);
			print("else {\n");
			codeSTMT(decl->val.defaultC, indentation + 1);
			printIndentation(indentation);
			print("}");
		} else {
			codeSTMT(decl->val.defaultC, indentation);
		}
		NEWLINE;
	}
}

void codeFOR_CLAUSE(FOR_CLAUSE *obj) {
	print("for(");
	if (obj->init_stmt) {
		codeSTMT(obj->init_stmt, 0);
	}
	SEMICOLON;
	SPACE;
	if (obj->condition) {
		codeEXPsingle(obj->condition);
	}
	SEMICOLON;
	SPACE;
	if (obj->post_stmt) {
		codeSTMT(obj->post_stmt, 0);
	}
	print(") ");
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
		SEMICOLON; NEWLINE;
	}

	if (indentation) printIndentation(indentation);
	while (right->kind == parenK) {
		right = right->val.parenE;
	}
	if (right->kind != appendK) {
		codeEXPsingle(left);
		print(" = ");
	}
	codeEXPsingle(right);
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

		// Use `.at()` to get bound checks
		case indexK:
			codeEXP(obj->val.indexE.exp);
			print(".at(");
			codeEXP(obj->val.indexE.index);
			print(")");
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
			codeID(obj->val.appendE.id);
			print(".push_back(");
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
void codeEXPdefaultValueWithEqual(TYPE *type) {
	if (!type) return;

	switch (type->kind) {
		case type_refK:
			codeEXPdefaultValueWithEqual(type->val.refT->symbol->val.type);
			return;
		case type_intK:
		case type_floatK:
		case type_boolK:
		case type_stringK:
		case type_runeK:
			print(" = ");
			codeEXPdefaultValue(type);
			break;
		case type_sliceK:
			// Slice are not initialized in golang.
			break;
		case type_arrayK:
			print(" = {");
			codeEXPdefaultValue(type->val.arrayT.type);
			print("}");
			break;
		case type_structK:
			break;
	}
}

void codeEXPdefaultValue(TYPE *type) {
	if (!type) return;

	switch(type->kind) {
		case type_refK:
			// Should not be called as thise are unwrapped in parent call.
			return;
		case type_intK:
			print("0");
			return;
		case type_floatK:
			print("0.0");
			break;
		case type_boolK:
			print("true");
			break;
		case type_runeK:
			print("0");
			break;
		case type_stringK:
			print("\"\"");
			return;
		case type_arrayK:
			print("{");
			codeEXPdefaultValue(type->val.arrayT.type);
			print("}");
			break;
		case type_sliceK:
			// Slices are not initialized in golang
			break;
		case type_structK:
			// Don't need to init as they are done in the typedef.
			break;
	}
}
