#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myRedBlackTree.h"

REDBLACKNODE *makeRedBlackNode(char *name, VARIABLETYPE varType)
{
  REDBLACKNODE *node = malloc(sizeof(REDBLACKNODE));
  node->blackColor = 1;
  node->isLeftChild = 1;
  node->subType = "";
  node->varType = varType;
  node->name = name;

  return node;
}

REDBLACKNODE *makeRedBlackNodeWithSubType(char *name, VARIABLETYPE varType, char *subType)
{
  REDBLACKNODE *node = malloc(sizeof(REDBLACKNODE));
  node->blackColor = 1;
  node->isLeftChild = 1;
  node->subType = subType;
  node->varType = varType;
  node->name = name;

  return node;
}

int TREE_addVariable(REDBLACKTREE *tree, char *name,  VARIABLETYPE varType)
{
  return TREE_addRedBlackNode(tree, makeRedBlackNode(name, varType));
}

int TREE_addVariableWithSubType(REDBLACKTREE *tree, char *name,  VARIABLETYPE varType, char *subType)
{
  return TREE_addRedBlackNode(tree, makeRedBlackNodeWithSubType(name, varType, subType));
}

int TREE_addRedBlackNode( REDBLACKTREE *tree, REDBLACKNODE *node)
{
  if(tree->root == NULL)
  {
    tree->root = node;
    return 1;
  }
  else
  {
    int toReturn = NODE_addNodeIfDoesNotExist(tree->root, node);
    if(tree->root->parent != NULL)
    {
      tree->root = tree->root->parent;
    }
    return toReturn;
  }
}

REDBLACKNODE *NODE_findNode(REDBLACKNODE *node, char *name)
{
  int compare = strcmp(name,node->name);

  if(compare < 0)
  {
    if(node->leftChild == NULL)
    {
      return NULL;
    }
    else
    {
      return NODE_findNode(node->leftChild, name);
    }
  }
  else if(compare > 0)
  {
    if(node->rightChild == NULL)
    {
      return NULL;
    }
    else
    {
      return NODE_findNode(node->rightChild, name);
    }
  }
  else
  {
    return node;
  }
}

REDBLACKNODE *TREE_findNode(REDBLACKTREE *tree, char *name)
{
  if( tree->root == NULL)
  {
    return NULL;
  }
  else
  {
    return NODE_findNode(tree->root, name);
  }
}

int NODE_addNodeIfDoesNotExist(REDBLACKNODE *node, REDBLACKNODE *newNode)
{
  int compare = strcmp(newNode->name, node->name);

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

int isRed(REDBLACKNODE *node)
{
  if(node->blackColor)
  {
    return 0;
  }
  return 1;
}

int isBlack(REDBLACKNODE *node)
{
  if(node->blackColor)
  {
    return 1;
  }
  return 0;
}

void setBlack(REDBLACKNODE *node)
{
  node->blackColor = 1;
}

void setRed(REDBLACKNODE *node)
{
  node->blackColor = 0;
}

void treeRotation(REDBLACKNODE *node)
{
  REDBLACKNODE *current = node;
  REDBLACKNODE *temp = NULL;

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

void NODE_printIndentation(int indentation)
{
  int i=0;
  for(i=0;i<indentation;i++)
  {
    printf("  ");
  }
}

void TREE_printTree(REDBLACKTREE *tree)
{
  printf("tree\n");
  printf("{\n");
  if(tree->root != NULL)
  {
    NODE_printNode(tree->root, 1);
  }
  printf("}\n");
}

void NODE_printNode(REDBLACKNODE *node, int indentation)
{
  NODE_printIndentation(indentation);
  printf("name: %s\n",node->name);
  NODE_printIndentation(indentation);
  printf("LeftChild");
  if(node->leftChild == NULL)
  {
    printf(" = NULL\n");
  }
  else
  {
    printf(":\n");
    NODE_printNode(node->leftChild, indentation+1);
  }
  NODE_printIndentation(indentation);
  printf("RightChild");
  if(node->rightChild == NULL)
  {
    printf(" = NULL\n");
  }
  else
  {
    printf(":\n");
    NODE_printNode(node->rightChild, indentation+1);
  }
}
