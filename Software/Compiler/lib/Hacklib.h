#include <stdio.h>

#if !defined(__HACK_LIB__)
#define __HACK_LIB__

struct __String__
{
    char *str;
    size_t size;
};

struct __Map3bit__
{ 
    const char *opc; 
    const char *bits;
};

struct __Map10bit__
{ 
    const char *opc; 
    const char *bits;
};

typedef struct __String__ String;
typedef struct __Map3bit__ Map3bit;
typedef struct __Map10bit__ Map10bit;

String extFile(const char *path);

String rmComment(String file);

String rmEmptySpace(String file);

String *extInstruction(String file);

char *newBinarySheet(String *file);

int Ainstruction(const String *file, char *buffer, int line);

int Cinstruction(const String *file, char *buffer, int line);

void freeExtInst(String *file);

int hackAssembler(const char *inpath, const char *outpath);

#endif // __HACK_LIB__