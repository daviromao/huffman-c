#include <stdio.h>
#include <stdlib.h>
#include "linked_list.c"

int INTERNAL_NODE = 256;

typedef struct PQNode PQNode;
typedef struct PQ PQ;
struct PQNode
{
    void *item;
    int priority;
    PQNode *next;
    PQNode *left;
    PQNode *right;
};

struct PQ
{
    PQNode *head;
    int size;
};

typedef struct ByteInfo
{
    unsigned char byte;
    int frequency;
    LL *bits;
} ByteInfo;

PQ *createPQ()
{
    PQ *newPQ = malloc(sizeof(PQ));
    newPQ->size = 0;
    newPQ->head = NULL;
    return newPQ;
}

void enQ(PQ *pq, void *item, int priority, PQNode *left, PQNode *right)
{
    PQNode *newNode = malloc(sizeof(PQNode));
    newNode->item = item;
    newNode->priority = priority;
    newNode->left = left;
    newNode->right = right;
    if (pq->head == NULL || priority <= pq->head->priority)
    {
        newNode->next = pq->head;
        pq->head = newNode;
    }
    else
    {
        PQNode *current = pq->head;
        while (current->next != NULL &&
               current->next->priority < priority)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    (pq->size)++;
}

PQNode *deQ(PQ *pq)
{
    if (pq->head == NULL)
    {
        return NULL;
    }
    else
    {
        PQNode *node = pq->head;
        pq->head = pq->head->next;
        node->next = NULL;
        (pq->size)--;
        return node;
    }
}

int minimumPriority(PQ *pq)
{
    if (pq->head == NULL)
    {
        return -1;
    }
    else
    {
        return pq->head->priority;
    }
}

void printPQ(PQ *q, void (*printFun)(void *))
{
    if (q->head == NULL)
    {
        printf("Empty Queue!\n");
        return;
    }
    PQNode *current = q->head;
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
    printf("%c ", *((int *)item));
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
PQNode *huffmanizeHead(PQ *pq)
{
    if (pq->head->next == NULL)
    {
        return deQ(pq);
    }

    PQNode *left = deQ(pq);
    PQNode *right = deQ(pq);

    enQ(pq, (void *)&INTERNAL_NODE, left->priority + right->priority, left, right);
    huffmanizeHead(pq);
}

void printNode(int b)
{
    if (b == 42)
    {
        printf("\\*");
    }
    else if (b == 92)
    {
        printf("\\\\");
    }
    else if (b == 256)
    {
        printf("*");
    }
    else
    {
        printf("%c", b);
    }
}

void printPreOrder(PQNode *bt)
{
    if (bt != NULL)
    {
        int byte = *(int *)bt->item;
        printNode(byte);
        printPreOrder(bt->left);
        printPreOrder(bt->right);
    }
}

int search(PQNode *bt, LL *l, void *item, char marker, int (*cmp_fun)(void *, void *))
{
    if (bt == NULL)
    {
        return 0;
    }

    printf("noatual = %c\n", *(unsigned char *)bt->item);
    if (cmp_fun(item, bt->item))
    {
        printf("acheiii!\n");
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        return 1;
    }
    int status;
    status = search(bt->left, l, item, '0', cmp_fun);
    if (status == 1)
    {
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        return status;
    }
    status = search(bt->right, l, item, '1', cmp_fun);
    if (status == 1)
    {
        if (marker != '2')
        {
            addToHead(marker, l);
        }
        return status;
    }

    return 0;
}

int cmpChar(void *a, void *b)
{
    unsigned char x = *(unsigned char *)a;
    unsigned char y = *(unsigned char *)b;
    return x == y;
}

void getMappedBits(PQNode *bt, ByteInfo bytes[])
{
    for (int i = 0; i < 256; i++)
    {
        if (bytes[i].frequency == 0)
        {
            continue;
        }
        LL *list = createLL();
        unsigned char c = bytes[i].byte;
        search(bt, list, (void *)&c, '2', cmpChar);
        bytes[i].bits = list;
    }

    for (int i = 0; i < 256; i++)
    {
        if (bytes[i].frequency == 0)
        {
            continue;
        }
        printf("%c:\n", bytes[i].byte);
        iterateOverNodes(bytes[i].bits);
        printf("\n");
    }
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
    Node *current = list->head;
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

void compress(ByteInfo bytes[], char *filename)
{
    FILE *f = fopen(filename, "rb");
    FILE *cf = fopen("compressed.huff", "wb");

    unsigned char c;
    char bitTray[8] = {'2'};
    int appendIndex = 0;

    while (fread(&c, sizeof(char), 1, f) > 0)
    {
        Node *bits = bytes[c].bits->head;
        printf("\nbyte=%c\n", c);
        while (bits != NULL)
        {
            if (appendIndex == 8)
            {
                unsigned char compressedChar = set_array_to_byte(bitTray);
                printf("chegou em 8, salvando no arquivo o byte %d\n--------\n", compressedChar);
                fwrite(&compressedChar, sizeof(char), 1, cf);
                appendIndex = 0;
                resetTray(bitTray);
                continue;
            }
            printf("bit=%c ai=%d\n", bits->value, appendIndex);
            bitTray[appendIndex++] = bits->value;
            bits = bits->next;
        }
    }
    if (appendIndex < 8)
    {
        printf("ai=%d\n", appendIndex);
        printf("tem lixo de tamanho: %d\n", 8 - appendIndex);
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
    printf("chegou em 8, salvando no arquivo o byte %d\n--------\n", compressedChar);
    fwrite(&compressedChar, sizeof(char), 1, cf);
    appendIndex = 0;
    resetTray(bitTray);

    fclose(f);
    fclose(cf);
}

int main(void)
{
    PQ *pq = createPQ();
    // int frequencyCount[256] = {0};
    ByteInfo bytes[256];
    initBytes(bytes);
    char *filename = "input_slide.txt";
    getFrequencies(filename, bytes);

    for (int i = 0; i < 256; i++)
    {
        if (bytes[i].frequency == 0)
            continue;

        // int *value = (int *)malloc(sizeof(int));
        // *value = i;
        enQ(pq, (void *)&bytes[i].byte, bytes[i].frequency, NULL, NULL);
    }

    printf("size=%d\n", pq->size);
    printPQ(pq, printInt);

    PQNode *bt_head = huffmanizeHead(pq);
    printPreOrder(bt_head);
    printf("\n");
    // unsigned char c = 'E';
    // LL l = createLL();
    // search(bt_head, &l, (void *)&c, '2', cmpChar);
    // iterateOverNodes(&l);
    getMappedBits(bt_head, bytes);
    compress(bytes, filename);
    return 0;
}