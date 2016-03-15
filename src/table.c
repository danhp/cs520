#include "table.h"

SYMBOL *blankSymbol;

SymbolTable *initSymbolTable() {
	REDBLACKTREE *newTree = NEW(REDBLACKTREE);
	SymbolTable *t = NEW(SymbolTable);
	t->tree = newTree;
	t->next = NULL;
	return t;
}

SymbolTable *scopeSymbolTable(SymbolTable *s) {
	SymbolTable *t = NEW(SymbolTable);
	t = initSymbolTable();
	t->next = s;
	return t;
}

SymbolTable *unscopeSymbolTable(SymbolTable *s, int line) {
	if (shouldPrint) {
		printFrame(s, line);
	}
	return s->next;
}

SYMBOL *putSymbol(SymbolTable *t, char *name, TYPE *type, YYLTYPE loc) {
	char *b = "_";
	if (strcmp(name, b) == 0) return blank();

	return TREE_addVariable(t->tree, name, type, loc);
}

SYMBOL *getSymbol(SymbolTable *t, char *name, YYLTYPE loc) {
	if (strcmp(name, "_") == 0) return blank();

	SYMBOL *node = TREE_findNode(t->tree, name);
	while (!node && t->next) {
		node = TREE_findNode(t->next->tree, name);
		t = t->next;
	}

	if (!node) {
		printErrorUndefined(name, loc);
	}

	return node;
}

int isInTopFrame(SymbolTable *table, char *name) {
	if (strcmp(name, "_") == 0) return 1;

	SYMBOL *node = node = TREE_findNode(table->tree, name);
	if (node){
		return 1;
	} else {
		return 0;
	}
}

SYMBOL *blank() {
	if (!blankSymbol) {
		blankSymbol = NEW(SYMBOL);
		blankSymbol->id  = "_";
		blankSymbol->kind = blankSym;
	}

	return blankSymbol;
}

/* PRINTING RELATED */
void printSymbolTable(SymbolTable *t) {
	TREE_printTree(t->tree);
	if(t->next) {
		printSymbolTable(t->next);
	}
}

void printFrame(SymbolTable *t, int line) {
	printSymbols(t->tree, line);
}
