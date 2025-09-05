#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Hacklib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "few arguments");
        return -1;
    }
    FILE_INFO *fp = removeEmptySpace(
                    removeComments(
                    extF(argv[1])
                    )
                    );
    FILE *file = fopen("filetrimed.asm", "w");
    fprintf(file, fp->fp);
    free(fp);
    fclose(file);
    return 0;
}