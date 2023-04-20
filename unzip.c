
#include <stdio.h>
#include <stdlib.h>

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

int isBitSet(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

Bt *recreateBt(unsigned char tree[], int *index)
{
    int item = tree[*index];
    Bt *new_bt;

    if(item=='\\')
    {
        new_bt = create_bt((void*)(tree+(*index)+1), NULL, NULL);
        *index+=2;
    }
    else if(item=='*')
    {
        new_bt = create_bt(NULL, NULL, NULL);
        *index+=1;
        new_bt->left = recreateBt(tree, index);
        new_bt->right = recreateBt(tree, index);
    }
    else
    {
        new_bt = create_bt((void*)(tree+(*index)), NULL, NULL);
        *index+=1;
    }
    return new_bt;
}

long getTotalSize(FILE *f)
{
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    return size;
}

void calculateTreeAndTrashSize(FILE *file, int *treeSize, int *trashSize)
{
    unsigned char firstByte;
    fread(&firstByte, sizeof(unsigned char), 1, file);
    unsigned char secondByte;
    fread(&secondByte, sizeof(unsigned char), 1, file);

    int mask = 0b00011111;

    *trashSize = firstByte >> 5;
    *treeSize = ((firstByte & mask) << 8) | secondByte;
}

void unzipFile(FILE *zipedFile, Bt *treeHead, long totalSize, int treeSize, int trashSize)
{
    int headerSize = 2 + treeSize;
    long compressedSize = totalSize - headerSize;

    char *unzipedFilename = "hello.txt";
    FILE *unzipped = fopen(unzipedFilename, "wb");

    Bt *indexBt = treeHead;
    for(long i = 0; i < compressedSize; i++)
    {
        unsigned char byte;
        fread(&byte, sizeof(unsigned char), 1, zipedFile);
        int bitsToRead = i == (compressedSize-1) ? 8-trashSize : 8;
        for(int j=7; j>=(8-bitsToRead); j--)
        {
            if(isBitSet(byte, j))
            {
                indexBt = indexBt->right;
            }else{
                indexBt = indexBt->left;
            }

            if((indexBt->left == NULL) && (indexBt->right == NULL))
            {
                fwrite((unsigned char*)indexBt->item, sizeof(unsigned char), 1, unzipped);
                indexBt = treeHead;
            }
        }
    }

    fclose(unzipped);
}

int main()
{
    char *filename = "utils/base.huff";
    FILE *zippedFile = fopen(filename, "rb");

    long totalSize = getTotalSize(zippedFile);

    int treeSize, trashSize;
    calculateTreeAndTrashSize(zippedFile, &treeSize, &trashSize);

    unsigned char preOrderTree[treeSize];
    fread(preOrderTree, sizeof(unsigned char), treeSize, zippedFile);
    
    int preOrderIndex = 0;
    Bt *tree = recreateBt(preOrderTree, &preOrderIndex);

    unzipFile(zippedFile, tree, totalSize, treeSize, trashSize);

    fclose(zippedFile);
}