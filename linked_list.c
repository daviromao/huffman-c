#include <stdio.h>
#include <stdlib.h>

typedef struct LLNode LLNode;
struct LLNode
{
    unsigned char value;
    LLNode *next;
};

typedef struct LL
{
    LLNode *head;
    LLNode *tail;
    int size;
} LL;

LL *createLL()
{
    LL *list = malloc(sizeof(LL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}
void addToHead(unsigned char value, LL *list)
{
    LLNode *n = malloc(sizeof(LLNode));
    n->value = value;
    n->next = list->head;
    list->head = n;
    if (list->tail == NULL)
    {
        list->tail = list->head;
    }
    list->size += 1;
}
void addToTail(unsigned char value, LL *list)
{
    LLNode *n = malloc(sizeof(LLNode));
    n->value = value;
    n->next = NULL;
    if (list->head == NULL && list->tail == NULL)
    {
        list->tail = n;
        list->head = n;
    }
    else
    {
        list->tail->next = n;
        list->tail = n;
    }
    list->size += 1;
}
int iterateOverNodes(LL *list)
{
    if (list->head == NULL)
    {
        printf("This list is empty!\n");
        return 0;
    }

    LLNode *p = list->head;
    while (p != NULL)
    {
        printf("HEAD (%p) -> Node(val=%c, nxt=%p)\n", p, p->value, p->next);
        p = p->next;
    }
    return 1;
}

int printLL(LL *list)
{
    if (list->head == NULL)
    {
        printf("This list is empty!\n");
        return 0;
    }

    LLNode *p = list->head;
    while (p != NULL)
    {
        printf("%d ", p->value);
        p = p->next;
    }
    printf("\n");
    return 1;
}