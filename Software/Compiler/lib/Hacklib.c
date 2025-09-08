#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <ctype.h>
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

static size_t var = 16;

static int isValidLabel(const char *s) {
     if (!s || !*s) return 0;

    unsigned char c = (unsigned char)*s;
    // first char: A–Z a–z _ . $ :
    if (!((c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') ||
           c == '_' || c == '.' || c == '$' || c == ':'))
        return 0;

    // rest: A–Z a–z 0–9 _ . $ :
    for (s++; *s; s++) {
        c = (unsigned char)*s;
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') ||
               c == '_' || c == '.' || c == '$' || c == ':'))
            return 0;
    }
    return 1;
}

int find_pos(const char *s, int ch) 
{
    const char *p = strchr(s, ch);   // first occurrence
    return p ? (int)(p - s) : -1;
}

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
    Node *searchNode =  list ? list->head : NULL;
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
    String buffer = (String){ (char *)malloc(file.size + 1), file.size };
    if (!buffer.str) { perror("extLabel: malloc"); return (String){NULL,0}; }
    buffer.str[file.size] = '\0';

    for (int i = 0; preSymboles[i].symbole; i++) addNode(list, preSymboles[i]);

    size_t didx = 0;
    int buffLines = 0, sizeLine = 0;

    for (size_t sidx = 0; sidx < file.size; sidx++)
    {
        if (file.str[sidx] == '(')
        {
            while (sidx < file.size && file.str[sidx] != '\n') { sidx++; sizeLine++; }
            if (sizeLine < 3) { fprintf(stderr,"invalid label: empty label not allowed\n"); return (String){NULL,0}; }
            if (file.str[sidx - 1] != ')') { fprintf(stderr,"invalid label: label must end with ')'\n"); return (String){NULL,0}; }

            char *str = (char *)malloc((size_t)sizeLine - 1);
            if (!str) { perror("extLabel: malloc"); return (String){NULL,0}; }
            memcpy(str, file.str + sidx - sizeLine + 1, (size_t)sizeLine - 2);
            str[sizeLine - 2] = '\0';

            for (size_t p = sidx - sizeLine + 1; p < sidx - 1; ++p) {
                if (file.str[p] == '(' || file.str[p] == ')') {
                    fprintf(stderr,"invalid label syntax: extra parentheses inside\n");
                    return (String){NULL,0};
                }
            }

            if (!isValidLabel(str)) {
                fprintf(stderr,"invalid label symboles: invalid typing\n");
                return (String){NULL,0};
            }

            if (searchLinkedList(list, str) != -1) {
                fprintf(stderr,"multiple label declaration: you have already declared (%s)\n", str);
                return (String){NULL,0};
            }

            addNode(list, (Label){ str, buffLines });
            sizeLine = 0;
        }
        else
        {
            while (sidx < file.size && file.str[sidx] != '\n')
                buffer.str[didx++] = file.str[sidx++];
            if (sidx < file.size) { buffer.str[didx++] = '\n'; buffLines++; }
        }
    }

    buffer.str[didx] = '\0';
    buffer.size = didx;

    char *tmp = realloc(buffer.str, buffer.size + 1);
    if (tmp) buffer.str = tmp;

    free(file.str);
    return buffer;
}

String convLabel(String file, LinkedList *list)
{
    // allocate a roomy buffer (roughly x2) for possible symbol→number expansion
    size_t cap = file.size * 2 + 1;
    String out = (String){ (char *)malloc(cap), cap };
    if (!out.str) { perror("convLabel: malloc"); return (String){NULL, 0}; }

    size_t w = 0;                 // write index into out.str
    size_t i = 0;                 // read index into file.str

    while (i < file.size) {
        if (file.str[i] == '@') {
            i++; // skip '@'

            // grab token until newline (or end)
            size_t start = i;
            while (i < file.size && file.str[i] != '\n') i++;
            size_t len = i - start;

            if (len == 0) { // "@\n" → invalid
                fprintf(stderr, "invalid A-instruction: empty after '@'\n");
                free(out.str);
                return (String){NULL, 0};
            }

            const char *tok = file.str + start;

            // detect pure decimal number: strtol must consume the whole token
            errno = 0;
            char *endptr = NULL;
            long num = strtol(tok, &endptr, 10);
            int is_number = (errno == 0 && endptr == tok + len);

            if (is_number) {
                // keep it as @<number>
                int n = snprintf(out.str + w, cap - w, "@%ld", num);
                if (n < 0 || (size_t)n >= cap - w) { free(out.str); return (String){NULL, 0}; }
                w += (size_t)n;
            } else {
                // treat as symbol
                char *sym = (char *)malloc(len + 1);
                if (!sym) { perror("convLabel: malloc sym"); free(out.str); return (String){NULL, 0}; }
                memcpy(sym, tok, len);
                sym[len] = '\0';

                if (!isValidLabel(sym)) {
                    fprintf(stderr, "invalid label symboles: invalid typing\n");
                    free(sym);
                    free(out.str);
                    return (String){NULL, 0};
                }

                int value = searchLinkedList(list, sym);
                if (value == -1) {                // new variable symbol
                    value = (int)var++;
                    if (addNode(list, (Label){ sym, value }) != 0) {
                        free(sym);
                        free(out.str);
                        return (String){NULL, 0};
                    }
                } else {
                    free(sym); // already in table
                }

                int n = snprintf(out.str + w, cap - w, "@%d", value);
                if (n < 0 || (size_t)n >= cap - w) { free(out.str); return (String){NULL, 0}; }
                w += (size_t)n;
            }

            // copy trailing newline if present
            if (i < file.size && file.str[i] == '\n') {
                if (w + 1 >= cap) { free(out.str); return (String){NULL, 0}; }
                out.str[w++] = '\n';
                i++; // consume '\n'
            }
        } else {
            // copy non-@ text verbatim (including newlines)
            if (w + 1 >= cap) { free(out.str); return (String){NULL, 0}; }
            out.str[w++] = file.str[i++];
        }
    }

    out.str[w] = '\0';
    out.size   = w;

    // shrink to fit (optional)
    char *tmp = realloc(out.str, w + 1);
    if (tmp) out.str = tmp;

    free(file.str);
    freeLinkedList(list); // symbol table no longer needed after conversion
    return out;
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
    const int offset = line * 17;

    /* make a writable copy of the line */
    char *str = (char *)malloc(file[line].size + 1);
    if (!str) { perror("Cinstruction: malloc"); return -1; }
    memcpy(str, file[line].str, file[line].size + 1);

    char *dest = NULL, *comp = NULL, *jmp = NULL;

    int pos = find_pos(str, '=');
    if (pos != -1) {
        str[pos] = '\0';
        dest = str;
        comp = str + pos + 1;
    } else {
        comp = str;
    }

    pos = find_pos(comp, ';');
    if (pos != -1) {
        comp[pos] = '\0';
        jmp = comp + pos + 1;
    }

    /* --- write comp (10 bits) to bits 15..6 --- */
    int index = 0;
    while (compmap[index].opc && strcmp(comp, compmap[index].opc) != 0) index++;
    if (!compmap[index].opc) {
        fprintf(stderr, "line %d: %s C-instruction Error: There is no comp opcode %s !\n",
                line, file[line].str, comp);
        free(str); return -1;
    }
    /* compmap bits[0..9] go to buffer[offset+15..offset+6] */
    for (int i = 0; i < 10; ++i)
        buffer[offset + 15 - i] = compmap[index].bits[i];

    /* --- write dest (3 bits) to bits 5..3 --- */
    if (dest) {
        index = 0;
        while (destmap[index].opc && strcmp(dest, destmap[index].opc) != 0) index++;
        if (!destmap[index].opc) {
            fprintf(stderr, "line %d: %s C-instruction Error: There is no dest opcode %s !\n",
                    line, file[line].str, dest);
            free(str); return -1;
        }
        /* bits order d1 d2 d3 -> positions 5,4,3 */
        buffer[offset + 5] = destmap[index].bits[0];
        buffer[offset + 4] = destmap[index].bits[1];
        buffer[offset + 3] = destmap[index].bits[2];
    } else {
        buffer[offset + 5] = '0';
        buffer[offset + 4] = '0';
        buffer[offset + 3] = '0';
    }

    /* --- write jmp (3 bits) to bits 2..0 --- */
    if (jmp) {
        index = 0;
        while (jmpmap[index].opc && strcmp(jmp, jmpmap[index].opc) != 0) index++;
        if (!jmpmap[index].opc) {
            fprintf(stderr, "line %d: %s C-instruction Error: There is no jmp opcode %s !\n",
                    line, file[line].str, jmp);
            free(str); return -1;
        }
        buffer[offset + 2] = jmpmap[index].bits[0];
        buffer[offset + 1] = jmpmap[index].bits[1];
        buffer[offset + 0] = jmpmap[index].bits[2];
    } else {
        buffer[offset + 2] = '0';
        buffer[offset + 1] = '0';
        buffer[offset + 0] = '0';
    }

    free(str);
    return 0;
}


int hackAssembler(const char *inpath, const char *outpath){
    LinkedList *list = newLinkedList();
    String extfile = extFile(inpath);
    if(extfile.str == NULL)
        return -1;
    String rmcomment = rmComment(extfile);
    if(rmcomment.str == NULL)
        return -1;
    String rmemptyspace = rmEmptySpace(rmcomment);
    if(rmemptyspace.str == NULL)
        return -1;
    String extlabel = extLabel(rmemptyspace ,list);
    if(extlabel.str == NULL)
        return -1;
    String convlabel = convLabel(extlabel ,list);
    if(convlabel.str == NULL)
        return -1;
    String *file = extInstruction(convlabel);
    if(file->str == NULL)
        return -1;
    char *buffer = newBinarySheet(file);
    if(buffer == NULL)
        return -1;
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
    if(fp == NULL)
    {perror("hackAssembler"); return -1;}
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