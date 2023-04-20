#include <stdio.h>

void printUChar(void *item)
{
    printf("%d\n", *((unsigned char *)item));
}

int cmpUChar(void *a, void *b)
{
    if (b == NULL || a == NULL)
    {
        return 0;
    }
    unsigned char x = *(unsigned char *)a;
    unsigned char y = *(unsigned char *)b;
    return x == y;
}

unsigned char setBit(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask | c;
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