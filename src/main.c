#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "weed.h"
#include "pretty.h"
#include "symbol.h"
#include "type.h"
#include "code.h"

int yyparse();

PROGRAM *program;
char *filename;

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: golite (parse | pretty | (symbol|dumpsymtab) | type | pptype | code) file\n");
		return 1;
	}

	filename = (char *) malloc (strlen (argv[2]) + 1);
	sprintf (filename, "%s", argv[2]);

	freopen(argv[2],"r",stdin);

	if (!yyparse()) {
		weedPROGRAM(program);
		if (strcmp(argv[1], "parse") == 0) {
			return 0;
		}

		if (strcmp(argv[1], "pretty") == 0) {
			prettyPROGRAM(program, 0, 0);
			return 0;
		}

		if ((strcmp(argv[1], "symbol") == 0) || (strcmp(argv[1], "dumpsymtab") == 0)) {
			symPROGRAM(program, 1);
			return 0;
		} else {
			symPROGRAM(program, 0);
		}

		if (strcmp(argv[1], "type") == 0) {
			typePROGRAM(program);
		}

		if (strcmp(argv[1], "pptype") == 0) {
			typePROGRAM(program);
      prettyPROGRAM(program, 0, 1);
		}

    if (strcmp(argv[1], "code") == 0) {
      typePROGRAM(program);
      codePROGRAM(program, 0);
    }

		return 0;
	}
}
