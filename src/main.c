#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "weed.h"
#include "prettyTree.h"

int yyparse();

FILE *prettyFile;
FILE *prettyTree;

PROGRAM *program;

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("ERROR: No file was passed. \n");
    return 0;
  }

  freopen(argv[1],"r",stdin);

  if (!yyparse())
  {
    printf("Valid\n");
    weedPROGRAM(program);

    char *token;
    token = strtok(argv[1],".");
    char src[50], dest[50];
    strcpy(src, ".pretty.go");
    strcpy(dest, token);
    strcat(dest,src);

    if(argc == 3)
    {
      prettyTree = fopen("./prettyTree.tree","w+");
      treePROGRAM(program,0);
      fclose(prettyTree);
    }

    prettyFile = fopen(dest,"w+");
    prettyPROGRAM(program, 0);
    fclose(prettyFile);
    return 0;
  }
}
