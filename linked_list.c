#include <stdio.h>
#include <stdlib.h>

struct Node
{
    char value;
    struct Node *next;
};
typedef struct Node Node;

struct LL
{
    struct Node *head;
    struct Node *tail;
    int size;
};
typedef struct LL LL;

LL *createLL()
{
    LL *list = malloc(sizeof(LL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}
void addToHead(char value, LL *list)
{
    Node *n = malloc(sizeof(Node));
    n->value = value;
    n->next = list->head;
    list->head = n;
    if (list->tail == NULL)
    {
        list->tail = list->head;
    }
    list->size += 1;
}
void addToTail(char value, LL *list)
{
    Node *n = malloc(sizeof(Node));
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

    Node *p = list->head;
    while (p != NULL)
    {
        printf("HEAD (%p) -> Node(val=%c, nxt=%p)\n", p, p->value, p->next);
        p = p->next;
    }
    return 1;
}