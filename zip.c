#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/linkedlist.c"
#include "libs/utils.c"
#include "libs/huffman.c"
#define VERBOSE 0

int MAX_FILENAME_SIZE = 1200;

typedef struct ByteInfo
{
    unsigned char byte;
    int frequency;
    LL *bits;
} ByteInfo;

void initBytes(ByteInfo bytes[256])
{
    for (int i = 0; i < 256; i++)
    {
        bytes[i].byte = i;
        bytes[i].bits = NULL;
        bytes[i].frequency = 0;
    }
}

void getFrequencies(char *filename, ByteInfo bytes[256])
{
    FILE *f = fopen(filename, "rb");
    unsigned char c;

    while (fread(&c, sizeof(char), 1, f) > 0)
    {
        bytes[c].frequency += 1;
    }
    fclose(f);
}

void enQFromFrequencies(Huff *huff, ByteInfo bytes[256])
{
    for (int i = 0; i < 256; i++)
    {
        if (bytes[i].frequency == 0)
            continue;

        enQ(huff, (void *)&bytes[i].byte, bytes[i].frequency, NULL, NULL);
    }
}

void printHuffPreOrder(HuffNode *ht)
{
    if (ht != NULL)
    {
        if (ht->item == NULL)
        {
            printf("*");
        }
        else if (*(unsigned char *)ht->item == '*')
        {
            printf("\\*");
        }
        else if (*(int *)ht->item == '\\')
        {
            printf("\\\\");
        }
        else
        {
            printf("%c", *(unsigned char *)ht->item);
        }
        printHuffPreOrder(ht->left);
        printHuffPreOrder(ht->right);
    }
}

void saveHuffPreOrder(HuffNode *ht, LL *preOrderTree)
{
    if (ht != NULL)
    {
        if (ht->item == NULL)
        {
            addToTail(preOrderTree, '*');
        }
        else if (*(unsigned char *)ht->item == '*')
        {
            addToTail(preOrderTree, '\\');
            addToTail(preOrderTree, '*');
        }
        else if (*(int *)ht->item == '\\')
        {
            addToTail(preOrderTree, '\\');
            addToTail(preOrderTree, '\\');
        }
        else
        {
            addToTail(preOrderTree, *(unsigned char *)ht->item);
        }
        saveHuffPreOrder(ht->left, preOrderTree);
        saveHuffPreOrder(ht->right, preOrderTree);
    }
}

int searchByte(HuffNode *ht, LL *list, void *item, char marker,
               int (*cmpFun)(void *, void *))
{
    if (ht == NULL)
    {
        return 0;
    }

    if (cmpFun(item, ht->item))
    {
        if (marker)
            addToHead(list, marker);
        return 1;
    }

    if (searchByte(ht->left, list, item, '0', cmpFun))
    {
        if (marker)
            addToHead(list, marker);
        return 1;
    }

    if (searchByte(ht->right, list, item, '1', cmpFun))
    {
        if (marker)
            addToHead(list, marker);
        return 1;
    }

    return 0;
}

void getMappedBits(HuffNode *ht, ByteInfo bytes[])
{
    for (int i = 0; i < 256; i++)
    {

        if (bytes[i].frequency == 0)
            continue;

        LL *list = createLL();
        unsigned char c = bytes[i].byte;
        searchByte(ht, list, (void *)&c, 0, cmpUChar);
        bytes[i].bits = list;
    }
}

void setCompressedFilename(char *compressed, char *original)
{
    strcpy(compressed, original);
    char *ext = ".huff";
    strcat(compressed, ext);
}

void setHeaderBytes(unsigned char *headerBytes, int trashSize, int treeSize)
{
    int i = 5;
    while (trashSize != 0)
    {
        if (trashSize % 2 == 1)
        {
            headerBytes[0] = setBit(headerBytes[0], i);
        }
        i++;
        trashSize /= 2;
    }

    i = 0;
    int bytesIndex = 1;
    while (treeSize != 0)
    {
        if (i > 4 && bytesIndex == 0)
        {
            printf("header overflow. Tree is too big.\n");
            break;
        }

        if (i > 7)
        {
            i = 0;
            bytesIndex--;
        }

        if (treeSize % 2 == 1)
        {
            headerBytes[bytesIndex] = setBit(headerBytes[bytesIndex], i);
        }
        i++;
        treeSize /= 2;
    }
}

int zipTmpFile(char *filename, ByteInfo bytes[])
{
    FILE *original = fopen(filename, "rb");
    FILE *cf = fopen("tmp", "wb");

    unsigned char byte;
    unsigned char compressedByte = 0;
    int i = 7;
    while (fread(&byte, sizeof(char), 1, original) > 0)
    {
        LLNode *bits = bytes[byte].bits->head;

        while (bits != NULL)
        {
            if (i < 0)
            {
                fwrite(&compressedByte, sizeof(char), 1, cf);
                compressedByte = 0;
                i = 7;
            }

            if (bits->value == '1')
            {
                compressedByte = setBit(compressedByte, i);
            }

            i--;
            bits = bits->next;
        }
    }
    int trashSize = i + 1;
    fwrite(&compressedByte, sizeof(char), 1, cf);

    fclose(original);
    fclose(cf);
    return trashSize;
}

void zipFile(char *filename, ByteInfo bytes[], LL *preOrderTree)
{
    int trashSize = zipTmpFile(filename, bytes);

    unsigned char headerBytes[2] = {0};
    setHeaderBytes(headerBytes, trashSize, preOrderTree->size);

    char compressedFilename[MAX_FILENAME_SIZE];
    setCompressedFilename(compressedFilename, filename);

    FILE *tmp = fopen("tmp", "rb");
    FILE *compressed = fopen(compressedFilename, "wb");

    fwrite(headerBytes, sizeof(char), 2, compressed);

    LLNode *current = preOrderTree->head;
    while (current != NULL)
    {
        fwrite(&(current->value), sizeof(char), 1, compressed);
        current = current->next;
    }

    unsigned char fileBytes;
    while (fread(&fileBytes, sizeof(char), 1, tmp) > 0)
    {
        fwrite(&fileBytes, sizeof(char), 1, compressed);
    }

    fclose(tmp);
    fclose(compressed);
}

int main(void)
{
    char *filename = "utils/input_slide.txt";

    ByteInfo bytes[256];
    initBytes(bytes);

    Huff *huff = createHuff();

    // Read frequencies
    getFrequencies(filename, bytes);

    // Populate PriorityQueue
    enQFromFrequencies(huff, bytes);

    if (VERBOSE)
    {
        printf("---------------------- Huff Priority Q ----------------------\n");
        printfHuffQ(huff, printUChar);
    }

    // Get huffman tree
    HuffNode *ht_head = huffmanizeQ(huff);
    LL *preOrderTree = createLL();
    saveHuffPreOrder(ht_head, preOrderTree);

    if (VERBOSE)
    {
        printf("------------------------- Huff Tree -------------------------\n");
        printLL(preOrderTree);
    }

    // Map tree
    getMappedBits(ht_head, bytes);

    // Compress to file
    zipFile(filename, bytes, preOrderTree);
    printf("Arquivo comprimido para %s.huff!\n", filename);
    return 0;
}