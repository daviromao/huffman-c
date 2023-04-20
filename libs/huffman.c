#include <stdio.h>
#include <stdlib.h>

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

Huff *createHuff()
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

void printfHuffQ(Huff *huff, void (*printFun)(void *))
{
    if (huff->head == NULL)
    {
        printf("Empty Huffman Queue!\n");
        return;
    }
    HuffNode *current = huff->head;
    while (current != NULL)
    {
        printf("(priority=%d) ", current->priority);
        printFun(current->item);
        current = current->next;
    }
    printf("\n");
}

HuffNode *huffmanizeQ(Huff *huff)
{
    if (huff->head->next == NULL)
    {
        return deQ(huff);
    }

    HuffNode *left = deQ(huff);
    HuffNode *right = deQ(huff);

    enQ(huff, NULL, left->priority + right->priority, left, right);
    huffmanizeQ(huff);
}
