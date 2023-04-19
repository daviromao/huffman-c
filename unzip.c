
#include <stdio.h>
#include <stdlib.h>

int INTERNAL_NODE = 256;
typedef struct Bt Bt;

struct Bt
{
    void *item;
    Bt *left;
    Bt *right;
};

Bt *create_bt(void *item, Bt *left, Bt *right)
{
    Bt *new_bt = (Bt *) malloc(sizeof(Bt));
    new_bt->item = item;
    new_bt->left = left;
    new_bt->right = right;

    return new_bt;
}

Bt *recreate_bt(char *c, int *current)
{
    int item = *(c+*current);
    Bt *new_bt;

    if(item=='\\')
    {
        new_bt = create_bt((void*)(c+(*current)+1), NULL, NULL);
        *current+=2;
    }
    else if(item=='*')
    {
        new_bt = create_bt((void*)&INTERNAL_NODE, NULL, NULL);
        *current+=1;
        new_bt->left = recreate_bt(c, current);
        new_bt->right = recreate_bt(c, current);
    }
    else
    {
        new_bt = create_bt((void*)(c+*current), NULL, NULL);
        *current+=1;
    }

    return new_bt;
}

void printPreOrder(Bt *bt)
{
    if (bt!=NULL)
    {
        int item = *(int *)bt->item;
        if(item==INTERNAL_NODE)
        {
            item='*';
        }
        else if(item=='*')
        {
            printf("\\");
        }
        printf("%c", item);
        printPreOrder(bt->left);
        printPreOrder(bt->right);
    }
}

int main()
{
    char *filename = "utils/base.huff";
    FILE *compacted = fopen(filename, "rb");

    unsigned char firstByte;
    fread(&firstByte, sizeof(unsigned char), 1, compacted);
    unsigned char secondByte;
    fread(&secondByte, sizeof(unsigned char), 1, compacted);

    int trash = firstByte >> 5;
    int mask = 0b00011111;
    int treeSize = ((firstByte & mask) << 8) | secondByte;
    
    printf("trash: %d\n", trash);
    printf("treesize: %d\n", treeSize);
    printf("tree: ");

    unsigned char *c;
    fread(c, sizeof(unsigned char), treeSize, compacted);
    
    int current = 0;
    Bt *mybt = recreate_bt(c, &current);
    printPreOrder(mybt);

    fclose(compacted);
}