#include <stdio.h>

#if !defined(__HACK_LIB__)
#define __HACK_LIB__

typedef struct __LABEL__
{
    char *symbole;
    size_t value;
}Label;

typedef struct __NODE__
{
    struct __NODE__ *next;
    Label data;
}Node;

typedef struct __LINKEDLIST__
{
    Node *head;
    Node *tail;
    size_t size;
}LinkedList;

typedef struct __String__
{
    char *str;
    size_t size;
}String;


typedef struct __Map3bit__
{ 
    const char *opc; 
    const char *bits;
}Map3bit;

typedef struct __Map10bit__
{ 
    const char *opc; 
    const char *bits;
}Map10bit;

LinkedList *newLinkedList(void);

int addNode(LinkedList *list, Label data);

int searchLinkedList(const LinkedList *list, const char* symbole);

void printLinkedList(const LinkedList *list);

void freeLinkedList(LinkedList *list);

String extFile(const char *path);

String rmComment(String file);

String rmEmptySpace(String file);

String extLabel(String file, LinkedList *list);

String *extInstruction(String file);

char *newBinarySheet(String *file);

int Ainstruction(const String *file, char *buffer, int line);

int Cinstruction(const String *file, char *buffer, int line);

void freeExtInst(String *file);

int hackAssembler(const char *inpath, const char *outpath);

#endif // __HACK_LIB__