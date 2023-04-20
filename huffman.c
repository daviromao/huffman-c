#include <stdio.h>
#include <stdlib.h>
#include "linked_list.c"

int INTERNAL_NODE = 256;

typedef struct HuffNode HuffNode;
typedef struct Huff Huff;
struct HuffNode
{
    void *item;
    int priority;
    HuffNode *next;
    HuffNode *left;
    HuffNode *right;
};

struct Huff
{
    HuffNode *head;
    int size;
};

typedef struct ByteInfo
{
    unsigned char byte;
    int frequency;
    LL *bits;
} ByteInfo;

Huff *createPQ()
{
    Huff *newHuff = malloc(sizeof(Huff));
    newHuff->size = 0;
    newHuff->head = NULL;
    return newHuff;
}

void enQ(Huff *huff, void *item, int priority, HuffNode *left, HuffNode *right)
{
    HuffNode *newNode = malloc(sizeof(HuffNode));
    newNode->item = item;
    newNode->priority = priority;
    newNode->left = left;
    newNode->right = right;
    if (huff->head == NULL || priority <= huff->head->priority)
    {
        newNode->next = huff->head;
        huff->head = newNode;
    }
    else
    {
        HuffNode *current = huff->head;
        while (current->next != NULL &&
               current->next->priority < priority)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    (huff->size)++;
}

HuffNode *deQ(Huff *huff)
{
    if (huff->head == NULL)
    {
        return NULL;
    }
    else
    {
        HuffNode *node = huff->head;
        huff->head = huff->head->next;
        node->next = NULL;
        (huff->size)--;
        return node;
    }
}

int minimumPriority(Huff *huff)
{
    if (huff->head == NULL)
    {
        return -1;
    }
    else
    {
        return huff->head->priority;
    }
}

void printHuff(Huff *huff, void (*printFun)(void *))
{
    if (huff->head == NULL)
    {
        printf("Empty Huffman Queue!\n");
        return;
    }
    HuffNode *current = huff->head;
    while (current != NULL)
    {
        printFun(current->item);
        printf("(p=%d)\n", current->priority);
        current = current->next;
    }
    printf("\n");
}

void printInt(void *item)
{
    printf("%d ", *((unsigned char *)item));
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

// Isso aqui funciona só para a adição de 1 novo nó.
// Tem que aplicar para todos, até que reste só um (que vai ser a root)
HuffNode *huffmanizeHead(Huff *huff)
{
    if (huff->head->next == NULL)
    {
        return deQ(huff);
    }

    HuffNode *left = deQ(huff);
    HuffNode *right = deQ(huff);

    enQ(huff, (void *)&INTERNAL_NODE, left->priority + right->priority, left, right);
    huffmanizeHead(huff);
}
int isBitSet(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}
void printByte(unsigned char byte)
{
    int i = 0;
    for (int i = 7; i >= 0; i--)
    {
        if (isBitSet(byte, i))
        {
            printf("%d", 1);
        }
        else
        {
            printf("%d", 0);
        }
    }
    printf("\n");
}

void printNode(void *b)
{
    if (*(unsigned char *)b == 42)
    {
        printf("\\*");
    }
    else if (*(unsigned char *)b == 92)
    {
        printf("\\\\");
    }
    else if (*(int *)b == 256)
    {
        printf("*");
    }
    else
    {
        printf("%d", *(unsigned char *)b);
    }
}

void printPreOrder(HuffNode *ht)
{
    if (ht != NULL)
    {
        // int byte = *(int *)ht->item;
        // printNode(byte);
        printNode(ht->item);
        // printf("%d ", ()ht->item);
        printPreOrder(ht->left);
        printPreOrder(ht->right);
    }
}

void saveNode(int b, LL *tree)
{
    if (b == 42)
    {
        addToTail('\\', tree);
        addToTail('*', tree);
        // printf("\\*");
    }
    else if (b == 92)
    {
        addToTail('\\', tree);
        addToTail('\\', tree);
        // printf("\\\\");
    }
    else if (b == 256)
    {
        addToTail('*', tree);
        // printf("*");
    }
    else
    {
        addToTail(b, tree);
        // printf("%c", b);
    }
}

void saveHuffPreOrder(HuffNode *ht, LL *tree)
{
    if (ht != NULL)
    {
        int byte = *(int *)ht->item;
        saveNode(byte, tree);
        saveHuffPreOrder(ht->left, tree);
        saveHuffPreOrder(ht->right, tree);
    }
}

int cmpChar(void *a, void *b)
{
    if (*(int *)b == INTERNAL_NODE)
    {
        return 0;
    }
    unsigned char x = *(unsigned char *)a;
    unsigned char y = *(unsigned char *)b;
    // printf("%d == %d\n", x, y);
    return x == y;
}

int searchForByte(HuffNode *ht, LL *l, void *item, char marker,
                  int (*cmp_fun)(void *, void *))
{
    if (ht == NULL)
    {
        return 0;
    }
    // printf("noatual = %d - %d\n", *(unsigned char *)ht->item, *(int *)ht->item);
    if (cmp_fun(item, ht->item))
    {
        // printf("acheiii\n");
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        return 1;
    }
    if (searchForByte(ht->left, l, item, '0', cmp_fun))
    {
        // printf("hahahah marker=%d and %c\n", marker, marker);
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        // printf("hahahahahsssssss\n");
        return 1;
    }
    if (searchForByte(ht->right, l, item, '1', cmp_fun))
    {
        // printf("hahahah marker=%d and %c\n", marker, marker);
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        return 1;
    }

    return 0;
}

void getMappedBits(HuffNode *ht, ByteInfo bytes[])
{
    for (int i = 0; i < 256; i++)
    {

        if (bytes[i].frequency == 0)
        {
            continue;
        }
        // printf(">>>>>>>i=%d\n", i);
        LL *list = createLL();
        unsigned char c = bytes[i].byte;
        // printByte(c);
        searchForByte(ht, list, (void *)&c, '2', cmpChar);
        // printf("%d %c- lista: ", c, c);
        // printLL(list);
        bytes[i].bits = list;
        // printf(".......\n");
    }
    // printf("\n");
    // for (int i = 0; i < 256; i++)
    // {
    //     if (bytes[i].frequency == 0)
    //     {
    //         continue;
    //     }
    //     printf("%c:\n", bytes[i].byte);
    //     iterateOverNodes(bytes[i].bits);
    //     printf("\n");
    // }
}

void initBytes(ByteInfo bytes[])
{
    for (int i = 0; i < 256; i++)
    {
        bytes[i].byte = i;
        bytes[i].bits = NULL;
        bytes[i].frequency = 0;
    }
}

void resetTray(char arr[])
{
    for (int i = 0; i < 8; i++)
    {
        arr[i] = '2';
    }
}

unsigned char set_array_to_byte(char a[])
{
    int i = 0;
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++)
    {
        if (a[i] == '1')
        {
            unsigned char mask = 1 << (7 - i);
            byte = byte | mask;
        }
    }
    return byte;
}

unsigned char set_list_to_byte(LL *list)
{
    int i = 0;
    unsigned char byte = 0;
    LLNode *current = list->head;
    while (current != NULL)
    {
        if (current->value == '1')
        {
            unsigned char mask = 1 << (7 - i);
            byte = byte | mask;
        }
        current = current->next;
        i++;
    }
    return byte;
}
unsigned char setBit(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask | c;
}

void setHeaderByte(unsigned char *headerByte, int trashSize, int treeSize)
{
    int i = 5;
    while (trashSize != 0)
    {
        if (trashSize % 2 == 1)
        {
            headerByte[0] = setBit(headerByte[0], i);
        }
        i++;
        // printf("%d\n", trashSize % 2);
        trashSize /= 2;
    }
    i = 0;
    int headerByteIndex = 1;
    while (treeSize != 0)
    {
        if (i > 4 && headerByteIndex == 0)
        {
            printf("header overflow. Tree is too big.\n");
            break;
        }
        if (i > 7)
        {
            i = 0;
            headerByteIndex--;
        }
        if (treeSize % 2 == 1)
        {
            headerByte[headerByteIndex] = setBit(headerByte[headerByteIndex], i);
        }
        i++;
        // printf("%d\n", treeSize % 2);
        treeSize /= 2;
    }
}

int compressToTmpFile(char *filename, ByteInfo bytes[])
{
    FILE *f = fopen(filename, "rb");
    FILE *cf = fopen("tmp.huff", "wb");

    unsigned char c;
    char bitTray[8] = {'2'};
    int appendIndex = 0;

    while (fread(&c, sizeof(char), 1, f) > 0)
    {
        LLNode *bits = bytes[c].bits->head;
        // printf("\nbyte=%c\n", c);
        while (bits != NULL)
        {
            if (appendIndex == 8)
            {
                unsigned char compressedChar = set_array_to_byte(bitTray);
                // printf("chegou em 8, salvando no arquivo o byte %d\n--------\n", compressedChar);
                fwrite(&compressedChar, sizeof(char), 1, cf);
                appendIndex = 0;
                resetTray(bitTray);
                continue;
            }
            // printf("bit=%c ai=%d\n", bits->value, appendIndex);
            bitTray[appendIndex++] = bits->value;
            bits = bits->next;
        }
    }
    int trashSize = 0;
    if (appendIndex < 8)
    {
        // printf("ai=%d\n", appendIndex);
        trashSize = 8 - appendIndex;
        // printf("lixo=%d bits\n", trashSize);
        while (appendIndex != 8)
        {
            bitTray[appendIndex++] = 0;
        }
    }
    else
    {
        printf("lixo é zero!\n");
    }
    unsigned char compressedChar = set_array_to_byte(bitTray);
    // printf("chegou em 8, salvando no arquivo o byte %d\n--------\n", compressedChar);
    fwrite(&compressedChar, sizeof(char), 1, cf);
    appendIndex = 0;
    resetTray(bitTray);
    fclose(f);
    fclose(cf);
    return trashSize;
}

void compressFile(char *filename, ByteInfo bytes[], LL *preOrderTree)
{
    int trashSize = compressToTmpFile(filename, bytes);

    unsigned char headerByte[2] = {0};
    setHeaderByte(headerByte, trashSize, preOrderTree->size);

    FILE *tmp = fopen("tmp.huff", "rb");
    FILE *compressed = fopen("compressed.huff", "wb");

    fwrite(headerByte, sizeof(char), 2, compressed);

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

    // for (int i = 0; i < 2; i++)
    // {
    //     printByte(headerByte[i]);
    // }
}

int main(void)
{
    Huff *huff = createPQ();
    ByteInfo bytes[256];
    initBytes(bytes);
    char *filename = "whitebackground.webp";

    // Read frequencies
    getFrequencies(filename, bytes);

    // Populate PriorityQueue
    for (int i = 0; i < 256; i++)
    {
        if (bytes[i].frequency == 0)
            continue;
        enQ(huff, (void *)&bytes[i].byte, bytes[i].frequency, NULL, NULL);
    }

    printf("---------------------- Huff Priority Q ----------------------\n");
    printHuff(huff, printInt);

    // Get huffman tree
    printf("----------------------- Preorder Tree -----------------------\n");
    HuffNode *ht_head = huffmanizeHead(huff);
    printPreOrder(ht_head);
    printf("\n");
    LL *preOrderTree = createLL();
    saveHuffPreOrder(ht_head, preOrderTree);
    // printLL(preOrderTree);
    // printf("sizeoftree=%d\n", preOrderTree->size);
    // printf("\n");

    // Map tree
    getMappedBits(ht_head, bytes);
    // printf("ppppppppppppppppp\n");
    // iterateOverNodes(bytes[0].bits);
    // Compress to file
    compressFile(filename, bytes, preOrderTree);
    return 0;
}