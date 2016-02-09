#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"

int yyparse();

PROGRAM *program;

int main(int argc, char *argv[]) {
	if (!yyparse()) {
		printf("Valid");
		return 0;
	}
}
