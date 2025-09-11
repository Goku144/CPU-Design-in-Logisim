#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

#if !defined(__HACK_LIB__)
#define __HACK_LIB__

typedef struct Pair {char *key; uint16_t value;} Pair; // any key value pair
typedef struct Node {struct Node *next; Pair data;} Node; // atomic block of the LinkedList
typedef struct LinkedList {Node *head; Node *tail; size_t size;} LinkedList; // Structure used to store labels
typedef struct String {char *str; size_t size;} String; // used for string manipulation
typedef struct InstructionMap {const char* instrcuction; const char* value;} InstructionMap; // use for instruction mapping
typedef struct CPair { const char *key; char value; } CPair; // key is const

static const CPair hexNumber[] = 
{
    {"0000",'0'}, {"0001",'1'}, {"0010",'2'}, {"0011",'3'},
    {"0100",'4'}, {"0101",'5'}, {"0110",'6'}, {"0111",'7'},
    {"1000",'8'}, {"1001",'9'}, {"1010",'A'}, {"1011",'B'},
    {"1100",'C'}, {"1101",'D'}, {"1110",'E'}, {"1111",'F'}
};

String hackAssembler(const char *inpath);

String convToHex(const String file);

#endif // __HACK_LIB__