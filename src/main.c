#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "weed.h"

int yyparse();

PROGRAM *program;

int main(int argc, char *argv[]) {
	if (!yyparse()) {
		printf("Valid\n");
    weedPROGRAM(program);
		return 0;
	}
}
