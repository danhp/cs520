#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"

void yyerror(char *msg);
void printError(char *msg, YYLTYPE loc);
