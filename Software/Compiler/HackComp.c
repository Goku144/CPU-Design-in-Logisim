#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Hacklib.h>

// String type to define it as array use
// String *string = malloc(n+1 * sizeof *string);
// now u have
// string[0] = {char *str, long size}
// string[1] = {char *str, long size}
// ...
// string[n] = {NULL, 0} terminator

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "few arguments");
        return -1;
    }
    String *file = extInstruction(rmEmptySpace(rmComment(extFile(argv[1])))); // its terminator is the null terminator
    
    char *buffer = newBinarySheat(file);
    int i = 0;
    while (file[i].str)
    {
        Ainstruction(file, buffer, i);
        i++;
    }

    printf("%s", buffer);
    return 0;
}