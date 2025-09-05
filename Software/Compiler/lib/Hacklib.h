#include <stdio.h>

#if !defined(__DATA_STRUCTURE__)
#define __DATA_STRUCTURE__

/*************************** struct def ***************************/

struct __FILE_INFO__
{
    char *fp;
    long size;
};

struct __Block__
{
    void *data;
    struct __Block__ *next;
    struct __Block__ *prev;
};

struct __LinearBlock__
{
    struct __Block__ *bottom; // bottom is the head
    struct __Block__ *top;    // top is the tail
    size_t size;
};

/**************************** type def ****************************/

typedef struct __FILE_INFO__ FILE_INFO;
typedef struct __Block__ Block;
typedef struct __LinearBlock__ Stack;
typedef struct __LinearBlock__ Queu;

/**************************** func def ****************************/

FILE_INFO *extF(char *path);

FILE_INFO *removeComments(FILE_INFO *file);

FILE_INFO *removeEmptySpace(FILE_INFO *file);

Stack *newStack(void);

Queu *newQueu(void);

void push(Stack *stack, void *data);

void enqueu(Queu *queu, void *data);

void *pop(Stack *stack);

void *dequeu(Queu *queu);

void destroyStack(Stack **stack);

void destroyQueu(Queu **queu);

#endif // __DATA_STRUCTURE__