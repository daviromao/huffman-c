#include <stdlib.h>

typedef struct BinaryTree BinaryTree;

struct BinaryTree
{
    void *item;
    BinaryTree *left;
    BinaryTree *right;
};

BinaryTree *createTree(void *item, BinaryTree *left, BinaryTree *right)
{
    BinaryTree *newBT = (BinaryTree *) malloc(sizeof(BinaryTree));
    newBT->item = item;
    newBT->left = left;
    newBT->right = right;

    return newBT;
}

BinaryTree *recreateTreeFromPreorder(unsigned char tree[], int *index)
{
    int item = tree[*index];
    BinaryTree *newBT;

    if(item=='\\')
    {
        newBT = createTree((void*)(tree+(*index)+1), NULL, NULL);
        *index+=2;
    }
    else if(item=='*')
    {
        newBT = createTree(NULL, NULL, NULL);
        *index+=1;
        newBT->left = recreateTreeFromPreorder(tree, index);
        newBT->right = recreateTreeFromPreorder(tree, index);
    }
    else
    {
        newBT = createTree((void*)(tree+(*index)), NULL, NULL);
        *index+=1;
    }
    return newBT;
}