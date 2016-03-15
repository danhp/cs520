#include "redblacktree.h"

SYMBOL *makeSYMBOL(char *id, SymbolKind kind, YYLTYPE loc) {
  SYMBOL *node = malloc(sizeof(SYMBOL));
  node->blackColor = 1;
  node->isLeftChild = 1;
  node->kind = kind;
  node->loc = loc;
  node->id = id;

  return node;
}

SYMBOL *TREE_addVariable(REDBLACKTREE *tree, char *id,  SymbolKind kind, YYLTYPE loc) {
  SYMBOL *s = makeSYMBOL(id, kind, loc);
  int err = TREE_addSYMBOL(tree, s);
  if(err == 0) {
    printErrorRedeclaration(loc, id, TREE_findNode(tree, id)->loc);
  }
  return s;
}

int TREE_addSYMBOL( REDBLACKTREE *tree, SYMBOL *node) {
  if(tree->root == NULL) {
    tree->root = node;
    return 1;
  } else {
    int toReturn = NODE_addNodeIfDoesNotExist(tree->root, node);
    if(tree->root->parent != NULL) {
      tree->root = tree->root->parent;
    }
    return toReturn;
  }
}

SYMBOL *NODE_findNode(SYMBOL *node, char *id) {
  int compare = strcmp(id,node->id);

  if(compare < 0) {
    if(node->leftChild == NULL) {
      return NULL;
    } else {
      return NODE_findNode(node->leftChild, id);
    }
  } else if(compare > 0) {
    if(node->rightChild == NULL) {
      return NULL;
    } else {
      return NODE_findNode(node->rightChild, id);
    }
  } else {
    return node;
  }
}

SYMBOL *TREE_findNode(REDBLACKTREE *tree, char *id) {
  if( tree->root == NULL) {
    return NULL;
  } else {
    return NODE_findNode(tree->root, id);
  }
}

int NODE_addNodeIfDoesNotExist(SYMBOL *node, SYMBOL *newNode)
{
  int compare = strcmp(newNode->id, node->id);

  if(compare < 0)
  {
    if(node->leftChild == NULL)
    {
      newNode->parent = node;
      newNode->isLeftChild = 1;
      node->leftChild = newNode;
      treeRotation(node);
      return 1;
    }
    else
    {
      return NODE_addNodeIfDoesNotExist(node->leftChild, newNode);
    }
  }
  else if(compare > 0)
  {
    if(node->rightChild == NULL)
    {
      newNode->parent = node;
      newNode->isLeftChild = 0;
      node->rightChild = newNode;
      treeRotation(node);
      return 1;
    }
    else
    {
      return NODE_addNodeIfDoesNotExist(node->rightChild, newNode);
    }
  }
  else
  {
    return 0;
  }
}

int isRed(SYMBOL *node)
{
  if(node->blackColor)
  {
    return 0;
  }
  return 1;
}

int isBlack(SYMBOL *node)
{
  if(node->blackColor)
  {
    return 1;
  }
  return 0;
}

void setBlack(SYMBOL *node)
{
  node->blackColor = 1;
}

void setRed(SYMBOL *node)
{
  node->blackColor = 0;
}

void treeRotation(SYMBOL *node)
{
  SYMBOL *current = node;
  SYMBOL *temp = NULL;

  if(node->parent == NULL)
  {
    return;
  }
  else if(isRed(node->parent))
  {
    treeRotation(node->parent);
  }

  if(isRed(node))
  {
    if(isRed(node->parent) && node->parent->parent == NULL)
    {
      setBlack(node->parent);
      return;
    }
    else if(node->isLeftChild && (node->parent->rightChild == NULL || isBlack(node->parent->rightChild)))
    {
      if(node->rightChild != NULL && isRed(node->rightChild))
      {
        temp = node->rightChild->leftChild;
        node->parent->leftChild = node->rightChild;
        node->rightChild->parent = node->parent;
        node->parent = node->rightChild;
        node->rightChild->leftChild = node;
        node->rightChild = temp;
        current = current->parent;
        current->isLeftChild = 1;
      }

      setBlack(current);
      current->isLeftChild = current->parent->isLeftChild;
      setRed(current->parent);
      current->parent->isLeftChild = 0;
      temp = current->parent->parent;
      current->parent->leftChild = current->rightChild;
      current->rightChild = current->parent;
      current->parent->parent = current;
      current->parent = temp;

      if(temp != NULL)
      {
        if(current->isLeftChild){ temp->leftChild = current; }
        else{ temp->rightChild = current; }
      }

      return;
    }
    else if(!node->isLeftChild && (node->parent->leftChild == NULL || isBlack(node->parent->leftChild)))
    {
      if(node->leftChild != NULL && isRed(node->leftChild))
      {
        temp = node->leftChild->rightChild;
        node->parent->rightChild = node->leftChild;
        node->leftChild->parent = node->parent;
        node->parent = node->leftChild;
        node->leftChild->rightChild = node;
        node->leftChild = temp;
        current = current->parent;
        current->isLeftChild = 0;
      }

      setBlack(current);
      current->isLeftChild = current->parent->isLeftChild;
      setRed(current->parent);
      current->parent->isLeftChild = 1;
      temp = current->parent->parent;
      current->parent->rightChild = current->leftChild;
      current->leftChild = current->parent;
      current->parent->parent = current;
      current->parent = temp;

      if(temp != NULL)
      {
        if(current->isLeftChild){ temp->leftChild = current; }
        else{ temp->rightChild = current; }
      }

      return;
    }
    else if(node->isLeftChild && isRed(node->parent->rightChild))
    {
      if(node->rightChild != NULL && isRed(node->rightChild))
      {
        temp = node->rightChild->leftChild;
        node->parent->leftChild = node->rightChild;
        node->rightChild->parent = node->parent;
        node->parent = node->rightChild;
        node->rightChild->leftChild = node;
        node->rightChild = temp;
        current = current->parent;
        current->isLeftChild = 1;
      }

      setBlack(current);
      setBlack(current->parent->rightChild);
      setRed(current->parent);
      treeRotation(current->parent);
      return;
    }
    else if(!node->isLeftChild && isRed(node->parent->leftChild))
    {
      if(node->leftChild != NULL && isRed(node->leftChild))
      {
        temp = node->leftChild->rightChild;
        node->parent->rightChild = node->leftChild;
        node->leftChild->parent = node->parent;
        node->parent = node->leftChild;
        node->leftChild->rightChild = node;
        node->leftChild = temp;
        current = current->parent;
        current->isLeftChild = 0;
      }

      setBlack(current);
      setBlack(current->parent->leftChild);
      setRed(current->parent);
      treeRotation(current->parent);
      return;
    }
  }
}

void NODE_printIndentation(int indentation) {
  int i=0;
  for(i=0;i<indentation;i++) {
    printf("\t");
  }
}

void TREE_printTree(REDBLACKTREE *tree) {
  printf("tree\n");
  printf("{\n");
  if(tree->root != NULL) {
    NODE_printNode(tree->root, 1);
  }
  printf("}\n");
}

void NODE_printNode(SYMBOL *node, int indentation) {
  NODE_printIndentation(indentation);
  printf("id: %s\n",node->id);
  NODE_printIndentation(indentation);
  printf("LeftChild");
  if(node->leftChild == NULL) {
    printf(" = NULL\n");
  } else {
    printf(":\n");
    NODE_printNode(node->leftChild, indentation+1);
  }

  NODE_printIndentation(indentation);
  printf("RightChild");
  if(node->rightChild == NULL) {
    printf(" = NULL\n");
  } else {
    printf(":\n");
    NODE_printNode(node->rightChild, indentation+1);
  }
}

void printSymbols(REDBLACKTREE *t, int line) {
	if (!t->root) return;

	printf("Exit at line %d\n", line);
	printf("--- FRAME ---\n");
	if (t->root) {
		printSyms(t->root);
	}
	printf("--- END ---\n\n");
}

void printSyms(SYMBOL *s) {
	printf("ID: %s, TYPE: %s\n", s->id, symbolTypeToString(s));

	if (s->leftChild) {
		printSyms(s->leftChild);
	}

	if (s->rightChild) {
		printSyms(s->rightChild);
	}
}

char *symbolTypeToString(SYMBOL *s) {
	char *string = "";
	char *msg = "Type Alias, ALIAS: ";
	char *temp = "";

	switch (s->kind) {
		case intSym:
			string = "int";
			break;
		case floatSym:
			string = "float64";
			break;
		case boolSym:
			string = "bool";
			break;
		case runeSym:
			string = "rune";
			break;
		case stringSym:
			string = "string";
			break;
		case arraySym:
			string = "array";
			break;
		case sliceSym:
			string = "slice";
			break;
		case structSym:
			string = "struct";
			break;
		case funcSym:
			string = "function";
			break;
		case typeSym:
			temp = (char *) malloc(strlen(typeToString(s->val.type)));
			sprintf(temp, "%s", typeToString(s->val.type));
			string = (char *) malloc(strlen(msg) + strlen(temp) + 1);
			sprintf(string, "%s%s", msg, temp);
			break;
		case inferredSym:
			string = "undefined";
			break;
		case blankSym:
			string = "BLANK";
			break;
	}

	return string ;
}

char *typeToString(TYPE *t) {
	char *string = "";
	switch (t->kind) {
		case type_refK:
			string = typeToString(t->val.refT.id->symbol->val.type);
			break;
		case type_intK:
			string = "int";
			break;
		case type_floatK:
			string = "float64";
			break;
		case type_boolK:
			string = "bool";
			break;
		case type_runeK:
			string = "rune";
			break;
		case type_stringK:
			string = "string";
			break;
		case type_arrayK:
			string = "array";
			break;
		case type_sliceK:
			string = "slice";
			break;
		case type_structK:
			string = "struct";
			break;
	}
	return string;
}