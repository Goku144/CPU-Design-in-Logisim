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

int hackAssembler(const char *inpath, const char *outpath);

#endif // __HACK_LIB__