#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hacklib.h"

String extFile(const char *path)
{
    FILE *fp = fopen(path, "rb"); // open the file for read
    String out = {NULL, 0};
    if (fp == NULL) {perror("extF: fopen"); return out;} // handle fopen error

    if(fseek(fp, 0, SEEK_END)) {perror("extF: fseek (end)"); fclose(fp); return out;} // seek end and handle error

    long size = ftell(fp); // tell the curent pointer position
    if(size == -1L) {perror("extF: ftell"); fclose(fp); return out;} // tell and handle error

    if(fseek(fp, 0, SEEK_SET)) {perror("extF: fseek (set)"); fclose(fp); return out;} // seek start and handle error

    size_t sz = (size_t) size;
    out.str = malloc(sz + 1);
    if (out.str == NULL) {perror("extF: malloc"); fclose(fp); return out;} // handle fopen error

    size_t n = fread(out.str, 1, sz, fp); // read the file put data inside mallocated space of out.str
    if (n != sz) // handle fread error
    {
        if (ferror(fp)) {perror("extF: fread"); free(out.str); out.str = NULL; fclose(fp); return out;}
        else if (feof(fp)) {out.size = n; out.str[n] = '\0'; fclose(fp); return out;}
    }

    fclose(fp);
    out.size = n;
    out.str[n] = '\0'; // terminator for string manipulations
    return out;
}

String rmComment(String file){
    for (size_t i = 0; i < file.size; i++)
    {
        if((i + 1) < file.size && file.str[i] == '/' && file.str[i + 1] == '/'){
            while (i < file.size && file.str[i] != '\n' && file.str[i] != '\r')
            {
                file.str[i++] = ' '; // replace it with whit space for triming stage
            }
        }    
    }
    return file;
}

String rmEmptySpace(String file){
    char *buffer = malloc(file.size + 1);
    if (buffer == NULL) {perror("rmEmptySpace: malloc"); return file;}
    size_t count = 0;
    for (size_t i = 0; i < file.size; i++)
    {
        if(file.str[i] != '\n' && file.str[i] != ' ' && file.str[i] != '\t' && file.str[i] != '\r') // ignore any leading '\n' '\t' '\r'
        {
            while (i < file.size && file.str[i] != '\n')
            {
                if(file.str[i] != '\t' && file.str[i] != '\r' && file.str[i] != ' ' )
                {   
                    buffer[count++] = file.str[i];
                }
                i++;
            }
            buffer[count++] = '\n';
        }
    }
    free(file.str);

    char *tmp = realloc(buffer, count + 1);
    if (!tmp) {free(buffer); perror("rmEmptySpace: realloc"); return file;}
    tmp[count] = '\0';

    file.str = tmp;
    file.size = count;
    return file;
}

// String type to define it as array use
// String *string = malloc(n+1 * sizeof *string);
// now u have
// string[0] = {char *str, long size}
// string[1] = {char *str, long size}
// ...
// string[n] = {NULL, 0} terminator

String *extInstruction(String file)
{
    size_t size = 0;
    for (size_t i = 0; i < file.size; i++)
    {
        if (file.str[i] == '\n')
            size++; 
    }

    String *buffer = malloc((size + 1) * sizeof *buffer );
    if (buffer == NULL) {perror("extInstruction: malloc"); return NULL;}

    size_t count = 0, pos = 0;
    for (size_t i = 0; i < size; i++)
    {
        while ((pos + count) < file.size && file.str[pos + count] != '\n')
            count++;
        buffer[i].str = malloc(count + 1);
        if (!buffer) { perror("extInstruction: malloc"); return NULL;}
        count = 0;

        while (pos < file.size && file.str[pos] != '\n')
        {
            buffer[i].str[count++] = file.str[pos++];
        }

        buffer[i].str[count] = '\0';
        buffer[i].size = count;
        count = 0;
        pos++;
    }

    buffer[size] = (String) {NULL, 1};
    free(file.str);
    return buffer;
}

char *newBinarySheat(String *file)
{
    size_t i = 0;
    size_t size = 0; 
    while (file[i].str)
    {   
        size++;
        i++;
    }
    size = size * 17;
    char *buffer = malloc( size + 1);
    if (buffer == NULL) {perror("newBinarySheat: malloc"); return NULL;}

    for (i = 0; i < size; i++)
    {
        if(i % 17 == 16)
            buffer[i] = '\n';
        else
            buffer[i] = '0';
    }
    buffer[size] = '\0';
    return buffer;
}

void Ainstruction(String *file, char *buffer, int line) // line is wich instruction line we are on ofsset
{ 
    int num, count = 0;
    int offset = line * 17;
    char *str =  malloc(file[line].size);
    strcpy(str, file[line].str + 1);
    num = atoi(str);

    while (num != 0)
    {
        buffer[15 + offset - count] = (char) ('0' + num % 2);
        num/=2;
        count++;
    }
    free(str);
}

void freeExtInst(String *file){
    int j = 0;
    while (file[j].str)
    {
        free(file[j++].str);
    }
    free(file);
}