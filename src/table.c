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

SYMBOL *putSymbol(SymbolTable *t, char *name, SymbolKind kind, YYLTYPE loc) {
	char *b = "_";
	if (strcmp(name, b) == 0) return blank();

	return TREE_addVariable(t->tree, name, kind, loc);
}

SYMBOL *getSymbol(SymbolTable *t, char *name) {
	if (strcmp(name, "_") == 0) return blank();

	SYMBOL *node = TREE_findNode(t->tree, name);
	if (!node) {
		if (t->next) {
			node = getSymbol(t->next, name);
		}
	}

	return node;
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