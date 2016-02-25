#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "weed.h"
#include "pretty.h"
int yyparse();

PROGRAM *program;

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: golite (parse | pretty) file\n");
		return 1;
	}

	freopen(argv[2],"r",stdin);

	if (!yyparse()) {
		weedPROGRAM(program);
		if (strcmp(argv[1], "parse") == 0) {
			return 0;
		}

		if (strcmp(argv[1], "pretty") == 0) {
			prettyPROGRAM(program, 0);
			return 0;
		}
		return 0;
	}
}
