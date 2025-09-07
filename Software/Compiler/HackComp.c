#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Hacklib.h>



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