
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

Bt *recreate_bt(char tree[], int *index)
{
    int item = tree[*index];
    Bt *new_bt;

    if(item=='\\')
    {
        new_bt = create_bt((void*)&tree[*index+1], NULL, NULL);
        *index+=2;
    }
    else if(item=='*')
    {
        new_bt = create_bt((void*)&INTERNAL_NODE, NULL, NULL);
        *index+=1;
        new_bt->left = recreate_bt(tree, index);
        new_bt->right = recreate_bt(tree, index);
    }
    else
    {
        new_bt = create_bt((void*)&tree[*index], NULL, NULL);
        *index+=1;
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

int is_bit_i_set(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

int main()
{
    char *filename = "utils/base.huff";
    FILE *compacted = fopen(filename, "rb");

    fseek(compacted, 0, SEEK_END);
    long totalSize = ftell(compacted);
    rewind(compacted);

    unsigned char firstByte;
    fread(&firstByte, sizeof(unsigned char), 1, compacted);

    unsigned char secondByte;
    fread(&secondByte, sizeof(unsigned char), 1, compacted);

    int trash = firstByte >> 5;
    int mask = 0b00011111;
    int treeSize = ((firstByte & mask) << 8) | secondByte;
    
    unsigned char preOrderTree[treeSize];
    fread(preOrderTree, sizeof(unsigned char), treeSize, compacted);
    int preOrderIndex = 0;
    Bt *mybt = recreate_bt(preOrderTree, &preOrderIndex);

    long compressedSize = totalSize - 2 - treeSize;


    char *unzipedFilename = "decompressed.txt";
    FILE *unziped = fopen(unzipedFilename, "wb");

    Bt indexBt = *mybt;
    for(long i = 0; i < compressedSize; i++){
        unsigned char byte;
        fread(&byte, sizeof(unsigned char), 1, compacted);

        int bitsToRead = i == (compressedSize-1) ? 8-trash : 8;

        for(int j=7; j>=(8-bitsToRead); j--){
            if(is_bit_i_set(byte, j)){
                indexBt = *indexBt.right;
            }else{
                indexBt = *indexBt.left;
            }

            if(*(int *)indexBt.item != INTERNAL_NODE){
                fwrite(indexBt.item, sizeof(unsigned char), 1, unziped);
                indexBt = *mybt;
            }
        }
    }

    fclose(compacted);
    fclose(unziped);
}