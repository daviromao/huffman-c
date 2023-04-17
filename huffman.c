#include <stdio.h>
#include <stdlib.h>

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
    printf("%d ", *((int *)item));
}

void getFrequencies(char *filename, int frequencies[256])
{
    FILE *f = fopen(filename, "rb");
    unsigned char c;

    while (fread(&c, sizeof(char), 1, f) > 0)
    {
        frequencies[c] += 1;
    }
    fclose(f);
}

// Isso aqui funciona só para a adição de 1 novo nó.
// Tem que aplicar para todos, até que reste só um (que vai ser a root)
void huffmanizeHead(PQ *pq, void *internalNodeDefault)
{
    PQNode *left = deQ(pq);
    PQNode *right = deQ(pq);
    enQ(pq, internalNodeDefault, left->priority + right->priority, left, right);
}

int main(void)
{
    PQ *pq = createPQ();
    int frequencyCount[256] = {0};
    char *filename = "bocchi.jpg";
    getFrequencies(filename, frequencyCount);

    // Isso é requerido por conta da implementação com void*
    int items[256];
    for (int i = 0; i < 256; i++)
    {
        items[i] = i;
    }

    // Enfileiramento
    for (int i = 0; i < 256; i++)
    {
        enQ(pq, (void *)&items[i], frequencyCount[i], NULL, NULL);
    }
    printf("size=%d\n", pq->size);
    printPQ(pq, printInt);

    // Essa função está incompleta
    short internalNodeDefault = 256;
    huffmanizeHead(pq, (void *)&internalNodeDefault);
    return 0;
}