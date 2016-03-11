#ifndef MYREDBLACKTREEHEADER
#define MYREDBLACKTREEHEADER

typedef enum
{
  VariableType_DNE,
  VariableType_int,
  VariableType_float,
  VariableType_string,
  VariableType_array,
  VariableType_struct,
  VariableType_rune,
  VariableType_boolean,
  VariableType_slice

}VARIABLETYPE;



typedef struct REDBLACKNODE
{
  VARIABLETYPE varType;
  char *name;
  char *subType;
  int blackColor;
  int isLeftChild;
  struct REDBLACKNODE *parent;
  struct REDBLACKNODE *rightChild;
  struct REDBLACKNODE *leftChild;
} REDBLACKNODE;

typedef struct REDBLACKTREE
{
  struct REDBLACKNODE *root;
} REDBLACKTREE;



REDBLACKNODE *makeRedBlackNode(char *name, VARIABLETYPE varType);
int TREE_addRedBlackNode( REDBLACKTREE *tree, REDBLACKNODE *node);
REDBLACKNODE *NODE_findNode(REDBLACKNODE *node, char *name);
REDBLACKNODE *TREE_findNode(REDBLACKTREE *tree, char *name);
int NODE_addNodeIfDoesNotExist(REDBLACKNODE *node, REDBLACKNODE *newNode);
void treeRotation(REDBLACKNODE *node);
int isRed(REDBLACKNODE *node);
int isBlack(REDBLACKNODE *node);
void setBlack(REDBLACKNODE *node);
void setRed(REDBLACKNODE *node);
int TREE_addVariable(REDBLACKTREE *tree, char *name,  VARIABLETYPE varType);
void TREE_printTree(REDBLACKTREE *tree);
void NODE_printNode(REDBLACKNODE *node, int indentation);
void NODE_printIndentation(int indentation);
int TREE_addVariableWithSubType(REDBLACKTREE *tree, char *name,  VARIABLETYPE varType, char *subType);
REDBLACKNODE *makeRedBlackNodeWithSubType(char *name, VARIABLETYPE varType, char *subType);

#endif
