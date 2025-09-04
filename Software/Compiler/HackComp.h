#include<stdio.h>

#if !defined(__DATA_STRUCTURE__)
#define __DATA_STRUCTURE__

struct __Node__
{
    void *data;
    struct __Node__ *next;
};

struct __DoubleNode__
{
    void *data;
    struct __DoubleNode__ *next;
    struct __DoubleNode__ *prev;
    size_t bytes;
};

struct __LinkedList__
{
    struct __Node__ *head;
    struct __Node__ *tail;
    int size;
};

struct __DoubleLinkedList__
{
    struct __DoubleNode__ *head;
    struct __DoubleNode__ *tail;
    int size;
};

typedef struct __Node__ Node;
typedef struct __DoubleNode__ DoubleNode;
typedef struct __LinkedList__ LinkedList;
typedef struct __DoubleLinkedList__ DoubleLinkedList;
typedef struct __DoubleLinkedList__ Stack;

LinkedList *createNewLinkedList();

DoubleLinkedList *createNewDoubleLinkedList();

Stack *createNewStack();
void addLinkedListNode(LinkedList *list, void *data);

void addDoubleLinkedListNode(DoubleLinkedList *list, void *data);

void push(Stack *stack, void *data);

void removeLinkedListNode(LinkedList *list);

void *removeDoubleLinkedListNode(DoubleLinkedList *list);

void *pop(Stack *stack);

void freeLinkedList(LinkedList *list);

void freeDoubleLinkedList(DoubleLinkedList *list);

void destroyStack(Stack *stack);


#endif // __DATA_STRUCTURE__