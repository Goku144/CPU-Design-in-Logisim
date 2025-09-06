#include <stdio.h>

#if !defined(__DATA_STRUCTURE__)
#define __DATA_STRUCTURE__

struct __String__
{
    char *str;
    size_t size;
};

typedef struct __String__ String;

String extFile(const char *path);

String rmComment(String file);

String rmEmptySpace(String file);

String *extInstruction(String file);

char *newBinarySheat(String *file);

void Ainstruction(String *file, char *buffer, int line);

void freeExtInst(String *file);

#endif // __DATA_STRUCTURE__