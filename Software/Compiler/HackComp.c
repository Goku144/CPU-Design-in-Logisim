#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <Hacklib.h>

// String type to define it as array use
// String *string = malloc(n+1 * sizeof *string);
// now u have
// string[0] = {char *str, long size}
// string[1] = {char *str, long size}
// ...
// string[n] = {NULL, 0} terminator

// returns the 0-based index of ch in s, or -1 if not found




int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "few arguments");
        return -1;
    }
    hackAssembler(argv[1], argv[2]);
    return 0;
}