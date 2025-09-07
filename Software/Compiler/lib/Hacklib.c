#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <regex.h>
#include "Hacklib.h"

/******************** static var ********************/

static const Map3bit destmap[] = 
{
    {"M", "001"}, {"D", "010"}, {"MD", "011"}, {"A", "100"}, 
    {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}, {NULL, "000"}
};

static const Map10bit compmap[] = 
{
    // for A
    {"0", "1110101010"}, {"1", "1110111111"}, {"-1", "1110111010"},
    {"D", "1110001100"}, {"A", "1110110000"}, {"!D", "1110001101"},
    {"!A", "1110110001"}, {"-D", "1110001111"}, {"-A", "1110110011"},
    {"D+1", "1110011111"}, {"A+1", "1110110111"}, {"D-1", "1110001110"},
    {"A-1", "1110110010"}, {"D+A", "1110000010"}, {"A+D", "1110000010"},
    {"D-A", "1110010011"}, {"A-D", "1110000111"}, {"D&A", "1110000000"},
    {"A&D", "1110000000"}, {"D|A", "1110010101"}, {"A|D", "1110010101"},
    // for M
    {"M",   "1111110000"}, {"!M",  "1111110001"}, {"-M", "1111110011"},
    {"M+1", "1111110111"}, {"M-1", "1111110010"}, {"D+M", "1111000010"},
    {"M+D", "1111000010"}, {"D-M", "1111010011"}, {"M-D", "1111000111"},
    {"D&M", "1111000000"}, {"M&D", "1111000000"}, {"D|M", "1111010101"},
    {"M|D", "1111010101"}, {NULL, "0000000000"}
};

static const Map3bit jmpmap[] = 
{
    {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"}, 
    {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}, {NULL, "000"}
};

static const Label preSymboles[] = 
{ 
    {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5},
    {"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9}, {"R10", 10}, {"R11", 11},
    {"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15}, {"SCREEN", 16384}, 
    {"KBO", 24576}, {"SP", 0}, {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4},
    {NULL, -1},
};

int find_pos(const char *s, int ch) 
{
    const char *p = strchr(s, ch);   // first occurrence
    return p ? (int)(p - s) : -1;
}

static size_t var = 16;

LinkedList *newLinkedList(void)
{
    LinkedList *list = malloc(sizeof *list);
    if(list == NULL){ perror("newLinkedList"); return NULL;}
    *list = (LinkedList) {0};
    return list;
}

int addNode(LinkedList *list, Label data)
{
    if (!list) {errno = EINVAL; perror("addNode"); return -1;}
    Node *newNode = malloc(sizeof *newNode);
    if(newNode == NULL){perror("addNode"); return -1;}

    newNode->data = data;
    newNode->next = NULL;
    if(list->head == NULL) {list->head = list->tail = newNode;}
    else
    {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
    return 0;
}

int searchLinkedList(const LinkedList *list, const char* symbole)
{
    Node *searchNode = list->head;
    while (searchNode)
    {
        if(strcmp(searchNode->data.symbole, symbole) == 0) return searchNode->data.value;
        searchNode = searchNode->next;
    }
    return -1;
}

void printLinkedList(const LinkedList *list)
{
    if (!list) {printf("(NULL)"); return;}
    Node *printNode = list->head;
    while (printNode)
    {
        printf("{%s, %lld}", printNode->data.symbole, printNode->data.value);
        printf(" -> ");
        printNode = printNode->next;
    }
    printf("NULL\n");
}

void freeLinkedList(LinkedList *list)
{
    if (!list) {printf("list is NULL\n"); return;}
    Node *freeNode = list->head;
    while (freeNode)
    {
        list->head = freeNode->next;
        free(freeNode);
        freeNode = list->head;
    }
    free(list);
}

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

String extLabel(String file, LinkedList *list)
{
    String buffer = (String){(char *) malloc(file.size + 1), file.size};
    buffer.str[file.size] = '\0';
    for (int i = 0; preSymboles[i].symbole; i++) addNode(list, preSymboles[i]);

    size_t didx = 0;
    int buffLines = 0, sizeLine = 0;
    for (size_t sidx = 0; sidx < file.size; sidx++)
    {
        if(file.str[sidx] == '(')
        {
            while (sidx < file.size && file.str[sidx] !='\n')
            {
                sidx++;
                sizeLine++;
            }
            if (sizeLine < 3) {fprintf(stderr, "invalid label: empty label not allowed\n"); return (String) {NULL, 0};}
            if (file.str[sidx - 1] != ')') {fprintf(stderr, "invalid label: label must end with ')'\n"); return (String) {NULL, 0};}

            char *str = malloc(sizeLine - 1);
            if(str == NULL) {perror("extLabel: malloc"); return (String) {NULL, 0};}
            memcpy(str, file.str + sidx - sizeLine + 1, sizeLine - 2);
            str[sizeLine - 2] = '\0';

            // part for regex verification
            for (size_t p = sidx - sizeLine + 1; p < sidx - 1; ++p) {
                if (file.str[p] == '(' || file.str[p] == ')') 
                {fprintf(stderr, "invalid label syntax: extra parentheses inside\n"); return (String){NULL, 0};}
            }

            if(searchLinkedList(list, str) != -1) {fprintf(stderr, "you have already declared (%s)\n", str); return (String) {NULL, 0};}

            addNode(list, (Label) {str, buffLines});
            sizeLine = 0;
        }
        else
        {
            while (sidx < file.size && file.str[sidx] != '\n')
            {
                buffer.str[didx++] = file.str[sidx++]; // beacuse buff lines give the exact place in which line the loop should be replaced by
            }
            buffer.str[didx++] = '\n';
            buffLines++;
        }
    }
    buffer.str[didx++] = '\n';
    buffer.str[didx] = '\0';
    buffer.size = didx;
    buffer.str = realloc(buffer.str, buffer.size);
    if(buffer.str == NULL) {perror("extLabel: realloc"); return (String) {NULL, 0};}
    
    free(file.str);
    return buffer;
}

String convLabel(String file, LinkedList *list)
{
    size_t buffsize = file.size * 2;
    String buffer = (String){(char *) malloc( buffsize + 1), file.size * 2};
    buffer.str[buffsize] = '\0';
    int linesize = 0, didx = 0;
    for (size_t sidx = 0; sidx < file.size; sidx++)
    {
        if(file.str[sidx] == '@')
        {
            while ((sidx + linesize) < file.size && file.str[sidx + linesize] != '\n')
            {
                linesize++;
            }
            file.str[linesize + sidx] = '\0';
            char *end = NULL;
            errno = 0;
            strtol(file.str + sidx + 1, &end, 10);
            if (*end != '\0') 
            {
                int labelvalue;
                if((searchLinkedList(list, (file.str + sidx + 1))) == -1) 
                {
                    char *str = malloc(linesize - 1);
                    if(str == NULL) {perror("convLabel: malloc"); return (String){NULL, 0};}
                    memcpy(str, file.str + sidx + 1, linesize - 1);
                    str[linesize - 1] = '\0';
                    addNode(list, (Label) {str, var++});
                }
                labelvalue = searchLinkedList(list, (file.str + sidx + 1));
                int len = snprintf(NULL, 0, "%lld", (size_t)labelvalue);   // length without '\0'
                char *s = malloc((size_t)len + 1);
                if (!s) return (String){NULL, 0};

                snprintf(s, (size_t)len + 1, "%lld", (size_t)labelvalue);  // <-- write into s

                buffer.str[didx++] = file.str[sidx];

                for (int pos = 0; pos < len; ++pos)           // <-- advance by index
                    buffer.str[didx++] = s[pos];

                buffer.str[didx++] = '\n';
                free(s);
            }
            file.str[linesize + sidx] = '\n';
            while (sidx < file.size && file.str[sidx] != '\n')
                sidx++;
            linesize = 0;
        }
        else
        {
            while (sidx < file.size && file.str[sidx] != '\n')
                buffer.str[didx++] = file.str[sidx++];
            buffer.str[didx++] = '\n';
        }
    }
    
    
    buffer.size = didx;
    buffer.str = realloc(buffer.str, buffer.size);
    if(buffer.str == NULL) {perror("convLabel: realloc"); return (String) {NULL, 0};}
    buffer.str[didx - 1] = '\0';
    free(file.str);
    //printf("%s", buffer.str);
    freeLinkedList(list);
    //printLinkedList(list); debug list
    return buffer;
}

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
        if (!buffer[i].str) { perror("extInstruction: malloc"); return NULL;}
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

char *newBinarySheet(String *file)
{
    size_t i = 0;
    size_t size = 0; 
    while (file[i].str)
    {   
        size++;
        i++;
    }
    size = size * 17;
    char *buffer = malloc(size);
    if (buffer == NULL) {perror("newBinarySheet: malloc"); return NULL;}

    for (i = 0; i < size - 1; i++)
    {
        if(i % 17 == 16)
            buffer[i] = '\n';
        else
            buffer[i] = '0';
    }
    buffer[size - 1] = '\0';
    return buffer;
}

int Ainstruction(const String *file, char *buffer, int line) // line is wich instruction line we are on offset
{ 
    size_t count = 0;
    size_t offset = line * 17;
    char *end = NULL;
    errno = 0;
    size_t num = (size_t) strtol(file[line].str + 1, &end, 10);

    if (end == file[line].str + 1) {fprintf(stderr, "line %d: %s A-instruction Error: no digit found!\n", line, file[line].str); return -1;}              // no digits
    if (errno == ERANGE) {fprintf(stderr, "line %d: %s A-instruction Error: out of long range!\n", line, file[line].str); return -1;}        // out of long range
    if (*end != '\0') {fprintf(stderr, "line %d: %s A-instruction Error: not a number!\n", line, file[line].str); return -1;}           // trailing junk (e.g., "123abc")
    if (num > 32767) {fprintf(stderr, "line %d: %s A-instruction Error: out of bound! (address must be 0..32767)\n", line, file[line].str); return -1;} // Hack A-instr range

    while (num != 0)
    {
        buffer[15 + offset - count] = (char) ('0' + num % 2);
        num /= 2;
        count++;
    }
    return 0;
}

int Cinstruction(const String *file, char *buffer, int line)
{
    int offset = line * 17;
    char *str = malloc(sizeof file[line].size + 1);
    strcpy(str, file[line].str);
    char *dest = NULL, *comp = NULL, *jmp = NULL;

    int pos = find_pos(str, '=');
    if (pos != -1)
    {
        str[pos] = '\0';
        dest = str;
        comp = str + pos + 1;
    }
    else
    {comp = str;}

    pos = find_pos(comp, ';');
    if(pos != -1)
    {
        comp[pos] = '\0';
        jmp = comp + pos + 1;
    }

    int count = 15;
    int index = 0;
    if(jmp)
    {
        while (jmpmap[index].opc != NULL)
        {
            if(strcmp(jmp, jmpmap[index].opc) == 0) break;
            index++;
        }
        if(jmpmap[index].opc == NULL){fprintf(stderr, "line %d: %s C-instruction Error: There is no jmp opcode %s !\n", line, file[line].str, jmp); return -1;}
        for (int i = 2; i >= 0; i--)
        {
            buffer[offset + count--] = jmpmap[index].bits[i];
        } 
        index = 0;
    }else{count-=3;}

    if(dest)
    {
        while (destmap[index].opc != NULL)
        {
            if(strcmp(dest, destmap[index].opc) == 0) break;
            index++;
        }
        if(destmap[index].opc == NULL){fprintf(stderr, "line %d: %s C-instruction Error: There is no dest opcode %s !\n", line, file[line].str, dest); return -1;}
        for (int i = 2; i >= 0; i--)
        {
            buffer[offset + count--] = destmap[index].bits[i];
        } 
        index = 0;
    }else{count-=3;}

    while (compmap[index].opc != NULL)
        {
            if(strcmp(comp, compmap[index].opc) == 0) break;
            index++;
        }
    if(compmap[index].opc == NULL){fprintf(stderr, "line %d: %s C-instruction Error: There is no comp opcode %s !\n", line, file[line].str, comp); return -1;}
    while (count >= 0)
    {
        buffer[offset + count] = compmap[index].bits[count];
        count--;
    }
    return 0;
}

int hackAssembler(const char *inpath, const char *outpath){
    LinkedList *list = newLinkedList();
    String *file = extInstruction(convLabel(extLabel(rmEmptySpace(rmComment(extFile(inpath))),list),list)); // its terminator is the null terminator
    char *buffer = newBinarySheet(file);
    int i = 0;
    while (file[i].str)
    {
        if(file[i].str[0] == '@')
        {
            if(Ainstruction(file, buffer, i)){ return 1;}
        }
        else
        {
            if(Cinstruction(file, buffer, i)){ return 1;}
        }
        i++;
    }
    FILE *fp = fopen(outpath, "w");
    fprintf(fp, "%s", buffer);
    fclose(fp);
    freeExtInst(file);
    return 0;
}

void freeExtInst(String *file){
    int j = 0;
    while (file[j].str)
    {
        free(file[j++].str);
    }
    free(file);
}