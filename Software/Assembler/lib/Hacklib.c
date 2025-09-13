#include "Hacklib.h"

// HACK API ASSEMBLER REQUESTS
_Static_assert(INT_MAX >= UINT16_MAX, "int too small to hold uint16_t");

static const InstructionMap destmap[] = 
{
    {"M",  "001"}, {"D",  "010"}, {"MD",  "011"}, {"A", "100"}, 
    {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}, {NULL, NULL}
};

static const InstructionMap compmap[] = 
{
    // for A
    {"0",   "1110101010"}, {"1",   "1110111111"}, {"-1",  "1110111010"},
    {"D",   "1110001100"}, {"A",   "1110110000"}, {"!D",  "1110001101"},
    {"!A",  "1110110001"}, {"-D",  "1110001111"}, {"-A",  "1110110011"},
    {"D+1", "1110011111"}, {"A+1", "1110110111"}, {"D-1", "1110001110"},
    {"A-1", "1110110010"}, {"D+A", "1110000010"}, {"A+D", "1110000010"},
    {"D-A", "1110010011"}, {"A-D", "1110000111"}, {"D&A", "1110000000"},
    {"A&D", "1110000000"}, {"D|A", "1110010101"}, {"A|D", "1110010101"},
    // for M
    {"M",   "1111110000"}, {"!M",  "1111110001"}, {"-M",  "1111110011"},
    {"M+1", "1111110111"}, {"M-1", "1111110010"}, {"D+M", "1111000010"},
    {"M+D", "1111000010"}, {"D-M", "1111010011"}, {"M-D", "1111000111"},
    {"D&M", "1111000000"}, {"M&D", "1111000000"}, {"D|M", "1111010101"},
    {"M|D", "1111010101"}, {NULL, NULL}
};

static const InstructionMap jmpmap[] = 
{
    {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"}, 
    {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}, {NULL, NULL}
};

static const Pair preSymboles[] = 
{ 
    {"R0",   0}, {"R1",         1}, {"R2",       2}, {"R3",   3}, {"R4",   4}, 
    {"R5",   5}, {"R6",         6}, {"R7",       7}, {"R8",   8}, {"R9",   9}, 
    {"R10", 10}, {"R11",       11}, {"R12",     12}, {"R13", 13}, {"R14", 14}, 
    {"R15", 15}, {"SCREEN", 16384}, {"KBD",  24576}, {"SP",   0}, {"LCL",  1}, 
    {"ARG",  2}, {"THIS",       3}, {"THAT",     4}, {NULL,  -1},
};

static uint16_t var = 16;

/********************************** Function Helpers **********************************/

static int find_pos(const char *s, int ch)
{
    const char *p = strchr(s, ch); // first occurrence
    return p ? (int)(p - s) : -1;
}

int isdigits(const char *string)
{
    if (!string || !*string)
        return -1;
    if (!string)
        return -1;
    errno = 0;
    char *end;
    unsigned long num = strtoul(string, &end, 10);

    if (end == string)
        return -1; // no digits
    if (*end != '\0')
        return -1; // trailing junk
    if (errno == ERANGE)
        return -1; // overflow
    if (num > 65535UL)
        return -1; // beyond uint16_t

    return (int)num;
}

int isValideLabel(const char *string)
{
    if (!string || !*string)
        return 0;
    // check leading char's
    if (!((string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') || string[0] == '_' || string[0] == '.' || string[0] == '$' || string[0] == ':'))
        return 0;
    for (size_t i = 1; string[i]; i++)
    {
        if (!((string[i] >= 'a' && string[i] <= 'z') || (string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= '0' && string[i] <= '9') || string[i] == '_' || string[i] == '.' || string[i] == '$' || string[i] == ':'))
            return 0;
    }
    return 1;
}

/********************************** Data Structure **********************************/

LinkedList *newLinkedList(void)
{
    LinkedList *list = malloc(sizeof(*list));
    if (!list)
    {
        perror("newLinkedList(malloc)");
        return NULL;
    }
    list->head = list->tail = NULL;
    list->size = 0;
    return list;
}

int addNode(LinkedList *list, Pair data)
{
    if (!list)
    {
        fprintf(stderr, "addNode Error: list is NULL\n");
        return -1;
    }
    Node *newNode = malloc(sizeof *newNode);
    if (!newNode)
    {
        perror("addNode(malloc)");
        return -1;
    }
    // add data to the Node
    newNode->next = NULL;
    newNode->data = data;
    if (!list->head)
        list->tail = list->head = newNode;
    else
    {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
    return 0;
}

int searchLinkedList(const LinkedList *list, const char *key)
{
    if (!list)
    {
        fprintf(stderr, "searchLinkedList Error: list is NULL\n");
        return -1;
    }
    Node *searchNode = list->head;
    while (searchNode)
    {
        if (strcmp(searchNode->data.key, key) == 0)
            return (int)searchNode->data.value;
        searchNode = searchNode->next;
    }
    return -1;
}

void printLinkedList(const LinkedList *list)
{
    if (!list)
        goto _return;
    Node *printNode = list->head;
    while (printNode)
    {
        printf("{%s, %hu} -> ", printNode->data.key, printNode->data.value);
        printNode = printNode->next;
    }
_return:
    printf("(NULL)\n");
}

void fprintLinkedList(const LinkedList *list)
{
    // Assembler step 3-2 output
    FILE *fp = fopen("..\\.AssemblerSteps\\3-2-SymbolesTree.json", "w");
    if (!fp)
        return;

    if (!list || !list->head)
    {
        fputs("[]", fp);
        fclose(fp);
        return;
    }

    Node *printNode = list->head;
    fprintf(fp, "[\n");

    while (printNode)
    {
        // JSON keys must be quoted; strings too
        fprintf(fp, "  {\"Symbole\":\"%s\",\"value\":%hu}",
                printNode->data.key ? printNode->data.key : "",
                (unsigned short)printNode->data.value);

        if (printNode->next)
            fprintf(fp, ",\n");
        else
            fprintf(fp, "\n");

        printNode = printNode->next;
    }

    fprintf(fp, "]");
    fclose(fp);
}

void freeLinkedList(LinkedList **list) // used if all key has been passed by value
{
    if (!list || !(*list))
    {
        fprintf(stderr, "freeLinkedList Error: list is NULL\n");
        return;
    }
    Node *freeNode = (*list)->head;
    while ((*list)->head)
    {
        freeNode = freeNode->next;
        free((*list)->head);
        (*list)->head = freeNode;
    }
    free(*list);
    *list = NULL;
}

void freeDeepLinkedList(LinkedList **list) // used if all key has been mallocated
{
    if (!list || !(*list))
    {
        fprintf(stderr, "freeDeepLinkedList Error: list is NULL\n");
        return;
    }

    int index = 0;
    while (index < 23)
    {
        (*list)->head = (*list)->head->next;
        index++;
    }
    Node *freeDeepNode = (*list)->head;
    while ((*list)->head)
    {
        if ((*list)->head->data.key)
            free((*list)->head->data.key);
        freeDeepNode = freeDeepNode->next;
        free((*list)->head);
        (*list)->head = freeDeepNode;
    }
    free(*list);
    *list = NULL;
}

/********************************** Assembler **********************************/

String extASMFile(const char *path)
{
    // initialize variables
    String buffer = (String){NULL, 0};
    char *memp = NULL;

    // opening the file
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        perror("extASMFile(fopen)");
        goto _return;
    }

    // extracting the file size
    if (fseek(fp, 0, SEEK_END))
    {
        perror("extASMFile(fseek(END))");
        goto _close;
    }
    long fs = ftell(fp);
    if (fs == -1L)
    {
        perror("extASMFile(ftell)");
        goto _close;
    }
    size_t size = (size_t)fs;
    if (fseek(fp, 0, SEEK_SET))
    {
        perror("extASMFile(fseek(START))");
        goto _close;
    }

    // mallocating buffer space
    memp = malloc(size + 1);
    if (!memp)
    {
        perror("extASMFile(malloc)");
        goto _close;
    }

    // read the file
    if (fread(memp, 1, size, fp) < size)
    {
        perror("extASMFile(fread)");
        free(memp);
        memp = NULL;
        goto _close;
    }

// closing the file
_close:
    if (fclose(fp) == EOF)
    {
        perror("extASMFile(fclose)");
        if (memp)
            free(memp);
        goto _return;
    }

    // assign value and EOF
    if (memp)
    {
        buffer = (String){memp, size};
        buffer.str[size] = '\0';
    }

// return from function call
_return:
    return buffer;
}

String trimASMFile(String file)
{
    // mallocating buffer space
    String buffer = (String){malloc(file.size + 1), file.size};
    if (!buffer.str)
    {
        perror("trimASMFile(malloc)");
        return file;
    }
    // starting search
    size_t dindx = 0;
    for (size_t sindx = 0; sindx < file.size; sindx++)
    {
        // checks for leading empty chars
        if (file.str[sindx] != '\r' && file.str[sindx] != '\n' && file.str[sindx] != '\t' && file.str[sindx] != ' ')
        {
            // remove outside comments
            if ((sindx + 1) < file.size && file.str[sindx] == '/' && file.str[sindx + 1] == '/')
            {
                while (sindx < file.size && file.str[sindx] != '\n')
                    sindx++;
                continue;
            }
            // check inside the line
            while (sindx < file.size && file.str[sindx] != '\n')
            {
                // remove inner comment
                if ((sindx + 1) < file.size && file.str[sindx] == '/' && file.str[sindx + 1] == '/')
                {
                    while (sindx < file.size && file.str[sindx] != '\n')
                        sindx++;
                    continue;
                }
                // remove any inside whit spaces
                if (file.str[sindx] == '\r' || file.str[sindx] == '\t' || file.str[sindx] == ' ')
                {
                    sindx++;
                    continue;
                }
                // insert valid char insid the buffer
                buffer.str[dindx++] = file.str[sindx++];
            }
            // add new line char
            buffer.str[dindx++] = '\n';
        }
    }
    // reallocate the new buffer space
    char *tmp = realloc(buffer.str, dindx + 1);
    if (!tmp)
    {
        perror("trimASMFile(realloc)");
        free(buffer.str);
        return file;
    }
    buffer = (String){tmp, dindx};
    buffer.str[dindx] = '\0';
    // free and return
    free(file.str);

    // Assembler step 1 output
    FILE *fp = fopen("..\\.AssemblerSteps\\1-trimASMFILE.asm", "w");
    if (!fp)
        return buffer;
    fprintf(fp, "%s", buffer.str);
    fclose(fp);

    return buffer;
}

String *extInstruction(String file)
{
    size_t indx = 0, lineSize = 0;
    // extracting number of lines
    while (indx < file.size)
    {
        if (file.str[indx] == '\n')
            lineSize++;
        indx++;
    }
    size_t dsize = 0, offset = 0;
    // mallocating the buffer array
    String *buffer = malloc((lineSize + 1) * sizeof *buffer);
    if (!buffer)
    {
        perror("extInstruction(malloc)");
        return NULL;
    }
    // retreve lines loop
    for (size_t line = 0; line < lineSize; line++)
    {
        // retreving the line size
        while (offset + dsize < file.size && file.str[offset + dsize] != '\n')
            dsize++;
        buffer[line].str = malloc(dsize + 1);
        if (!buffer[line].str)
        {
            perror("extInstruction(malloc)");
            while (line)
            {
                free(buffer[--line].str);
            }
            free(buffer);
            return NULL;
        }
        // copy data
        memcpy(buffer[line].str, file.str + offset, dsize);
        buffer[line].str[dsize] = '\0';
        buffer[line].size = dsize;
        offset += dsize + 1;
        dsize = 0;
    }

    buffer[lineSize] = (String){NULL, 0};
    free(file.str);

    // Assembler step 2 output
    FILE *fp = fopen("..\\.AssemblerSteps\\2-extInstruction.asm", "w");
    if (!fp)
        return buffer;
    int which = 0;
    size_t outindx = 0;
    while (buffer[outindx].str)
    {
        size_t inerindx = 0;
        fprintf(fp, "[");
        while (buffer[outindx].str[inerindx])
        {
            if (buffer[outindx].str[0] == '@')
                which = 1;
            else if (buffer[outindx].str[0] == '(')
                which = 2;
            fprintf(fp, "%c", buffer[outindx].str[inerindx]);
            inerindx++;
        }
        if (which == 1)
            fprintf(fp, "] -> //A instruction\n");
        else if (which == 2)
            fprintf(fp, "] -> //Label\n");
        else
            fprintf(fp, "] -> //C instruction\n");
        outindx++;
        which = 0;
    }
    fclose(fp);

    return buffer;
}

String *extLabel(String *file, LinkedList *list)
{
    size_t linesSize = 0;
    while (file[linesSize].str)
        linesSize++;

    String *buffer = malloc((linesSize + 1) * sizeof *buffer);
    if (!buffer)
    {
        perror("extLabel(malloc)");
        return NULL;
    }

    char *filelineChar;
    size_t bufferlinesSize = 0, filelineSize = 0, offline = 0;

    for (size_t fileline = 0; file[fileline].str; fileline++)
    {
        filelineChar = file[fileline].str;
        filelineSize = file[fileline].size;

        if (filelineChar[0] == '(')
        {
            if (filelineChar[filelineSize - 1] != ')')
            {
                fprintf(stderr, "(FILE: ../.AssemblerSteps/2-extLabel.asm) Label Syntax Error (line %zu:%zu): %s <- missing ')'\n", fileline + 1, filelineSize, filelineChar);
                while (bufferlinesSize)
                {
                    free(buffer[--bufferlinesSize].str);
                }
                free(buffer);
                return NULL;
            }
            if (filelineSize < 4)
            {
                fprintf(stderr, "(FILE: ../.AssemblerSteps/2-extLabel.asm) Label Empty Error (line %zu:%zu): %s you need to have at least (x)\n", fileline + 1, filelineSize, filelineChar);
                while (bufferlinesSize)
                {
                    free(buffer[--bufferlinesSize].str);
                }
                free(buffer);
                return NULL;
            }
            // terminate and check lable validity
            filelineChar[filelineSize - 1] = '\0';
            if (!isValideLabel(filelineChar + 1))
            {
                fprintf(stderr, "(FILE: ../.AssemblerSteps/2-extLabel.asm) Label Syntax Error (line %zu:%zu): %s invalid character('s) label must obey the regex general rule ^[A-Za-z_.$:][A-Za-z0-9_.$:]*$\n",
                        fileline + 1, filelineSize, filelineChar);
                return NULL;
            }
            // we need because we remove 2 from filelineSize and allocation start at 1 so (filelineSize - 2 + 1)
            char *str = malloc(filelineSize - 1);
            if (!str)
            {
                perror("extLabel(malloc)");
                while (bufferlinesSize)
                {
                    free(buffer[--bufferlinesSize].str);
                }
                free(buffer);
                return NULL;
            }
            // copy the string without pranthesize
            memcpy(str, filelineChar + 1, filelineSize - 1);
            addNode(list, (Pair){str, fileline - offline});
            offline++;
            continue;
        }
        // mallocating fo each buffer new line
        buffer[bufferlinesSize].str = malloc(filelineSize + 1);
        char *bufferlineChar = buffer[bufferlinesSize].str;
        if (!bufferlineChar)
        {
            perror("extLabel(malloc)");
            while (bufferlinesSize)
            {
                free(buffer[--bufferlinesSize].str);
            }
            free(buffer);
            return NULL;
        }

        // copy and terminate and incriment for next line
        memcpy(bufferlineChar, filelineChar, filelineSize);
        bufferlineChar[filelineSize] = '\0';
        buffer[bufferlinesSize].size = filelineSize;
        bufferlinesSize++;
    }
    String *tmp = realloc(buffer, (bufferlinesSize + 1) * sizeof *buffer);
    if (!tmp)
    {
        perror("extLabel(realloc)");
        while (bufferlinesSize)
        {
            free(buffer[--bufferlinesSize].str);
        }
        free(buffer);
        return NULL;
    }
    buffer = tmp;
    buffer[bufferlinesSize] = (String){NULL, 0};
    size_t indx = 0;
    while (file[indx].str)
    {
        free(file[indx++].str);
    }
    free(file);

    // Assembler step 3-1 output
    FILE *fp = fopen("..\\.AssemblerSteps\\3-1-extLabel.asm", "w");
    if (!fp)
        return buffer;
    int which = 0;
    size_t outindx = 0;
    while (buffer[outindx].str)
    {
        size_t inerindx = 0;
        fprintf(fp, "[");
        while (buffer[outindx].str[inerindx])
        {
            if (buffer[outindx].str[0] == '@')
                which = 1;
            fprintf(fp, "%c", buffer[outindx].str[inerindx]);
            inerindx++;
        }
        if (which == 1)
            fprintf(fp, "] -> //A instruction\n");
        else
            fprintf(fp, "] -> //C instruction\n");
        outindx++;
        which = 0;
    }
    fclose(fp);

    return buffer;
}

// you need to do line + 1
char *Ainstruction(LinkedList *list, const String line, const size_t numline)
{
    char *buffer = malloc(17), *end = NULL;
    if (!buffer)
    {
        perror("Ainstruction(malloc)");
        return NULL;
    }
    for (size_t i = 0; i < 16; i++)
        buffer[i] = '0';
    buffer[16] = '\0';

    size_t count = 0, num = 0;
    if (isdigits(line.str + 1) != -1)
    {
        errno = 0;
        num = (size_t)strtol(line.str + 1, &end, 10);
        if (errno == ERANGE)
        {
            fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: %s A-instruction Error: out of long range!\n", numline, line.str + 1);
            return NULL;
        } // out of long range
        if (num > 32767)
        {
            fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: %s A-instruction Error: out of bound! (address must be 0..32767)\n", numline, line.str + 1);
            return NULL;
        } // Hack A-instr range
    }
    else if (isValideLabel(line.str + 1))
    {
        int search = searchLinkedList(list, line.str + 1);
        if (search == -1)
        {
            char *bufferNode = malloc(line.size + 1);
            if (!bufferNode)
            {
                perror("Ainstruction(malloc)");
                return NULL;
            }
            memcpy(bufferNode, line.str + 1, line.size + 1);
            addNode(list, (Pair){bufferNode, var++});
        }
        num = searchLinkedList(list, line.str + 1);
    }
    else
    {
        fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: A-instruction Syntax Error: (%s) Invalid name for A variable \n", numline, line.str + 1);
        return NULL;
    } // Hack A-instr range
    while (num != 0)
    {
        buffer[15 - count] = (char)('0' + (num % 2));
        num /= 2;
        count++;
    }
    return buffer;
}

char *Cinstruction(String line, const size_t numline)
{
    char *buffer = malloc(17);
    if (!buffer)
    {
        perror("Cinstruction(malloc)");
        return NULL;
    }
    for (size_t i = 0; i < 16; i++)
        buffer[i] = '0';
    buffer[16] = '\0';

    char *dest = NULL, *comp = NULL, *jmp = NULL;
    int pos;
    if ((pos = find_pos(line.str, '=')) != -1)
    {
        dest = line.str;
        comp = line.str + pos + 1;
        line.str[pos] = '\0';
    }
    else
    {
        comp = line.str;
    }
    if ((pos = find_pos(comp, ';')) != -1)
    {
        jmp = comp + pos + 1;
        comp[pos] = '\0';
    }

    int compindx = 0;
    while (compmap[compindx].instrcuction && strcmp(compmap[compindx].instrcuction, comp) != 0)
        compindx++;
    if (!compmap[compindx].instrcuction)
    {
        fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: %s C-instruction Error: There is no computation opcode %s !\n", numline, line.str, comp);
        free(buffer);
        return NULL;
    }
    for (size_t cindx = 0; cindx < 10; cindx++)
        buffer[cindx] = compmap[compindx].value[cindx];

    int destindx = 0;
    if (dest)
    {
        while (destmap[destindx].instrcuction && strcmp(destmap[destindx].instrcuction, dest) != 0)
            destindx++;
        if (!destmap[destindx].instrcuction)
        {
            fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: %s C-instruction Error: There is no destination opcode %s !\n", numline, line.str, dest);
            free(buffer);
            return NULL;
        }
        for (size_t dindx = 0; dindx < 3; dindx++)
            buffer[dindx + 10] = destmap[destindx].value[dindx];
    }

    int jmpindx = 0;
    if (jmp)
    {
        while (jmpmap[jmpindx].instrcuction && strcmp(jmpmap[jmpindx].instrcuction, jmp) != 0)
            jmpindx++;
        if (!jmpmap[jmpindx].instrcuction)
        {
            fprintf(stderr, "(FILE: ../.AssemblerSteps/3-1-extLabel.asm) line %zu: %s C-instruction Error: There is no jump opcode %s !\n", numline, line.str, jmp);
            free(buffer);
            return NULL;
        }
        for (size_t jindx = 0; jindx < 3; jindx++)
            buffer[jindx + 13] = jmpmap[jmpindx].value[jindx];
    }

    return buffer;
}

String hackAssembler(const char *inpath)
{
    LinkedList *list = newLinkedList();
    String out = (String){NULL, 0};
    if (!list)
        return out;
    for (size_t i = 0; i < 23; i++)
        addNode(list, preSymboles[i]);
    String extasmfile = extASMFile(inpath);
    if (!extasmfile.str)
        return out;
    String trimasmfile = trimASMFile(extasmfile);
    if (!trimasmfile.str)
        return out;
    String *extinstruction = extInstruction(trimasmfile);
    if (!extinstruction)
        return out;
    String *file = extLabel(extinstruction, list);
    if (!file)
        return out;
    size_t lines = 0;
    while (file[lines].str)
        lines++;

    char *buffer = malloc(17 * lines);
    if (!buffer)
    {
        perror("Cinstruction(malloc)");
        return out;
    }

    char *str;
    size_t writeSize = 17 * lines, indx = 0;
    while (file[indx].str)
    {
        if (file[indx].str[0] == '@')
        {
            str = Ainstruction(list, file[indx], indx + 1);
        }
        else
        {
            str = Cinstruction(file[indx], indx + 1);
        }
        if (!str)
            return out;
        size_t destindx = 0;
        while (str[destindx])
        {
            buffer[indx * 17 + destindx] = str[destindx];
            destindx++;
        }
        buffer[indx * 17 + destindx] = '\n';
        indx++;
    }
    size_t freeindx = 0;
    while (file[freeindx].str)
    {
        free(file[freeindx].str);
        freeindx++;
    }
    free(file);
    fprintLinkedList(list);
    freeDeepLinkedList(&list);
    return (String){buffer, writeSize};
}

String convToHex(const String file)
{
    // Expect each line: 16 bits + '\n' => 17 bytes
    size_t lines = file.size / 17;
    String buffer = (String){malloc(lines * 5 + 1), lines * 5};
    if (!buffer.str)
    {
        perror("convToHex(malloc)");
        return (String){NULL, 0};
    }

    char binnum[4][5];
    char hexnum[5];

    for (size_t index = 0; index < lines; ++index)
    {
        size_t base = index * 17;

        // Build 4 nibbles (each 4 chars) from the 16-bit line
        for (size_t jndx = 0; jndx < 4; ++jndx)
        {
            for (size_t kndx = 0; kndx < 4; ++kndx)
                binnum[jndx][kndx] = file.str[base + jndx * 4 + kndx];
            binnum[jndx][4] = '\0';

            // Lookup nibble → hex
            int m = 0;
            for (; m < 16; ++m)
                if (strcmp(hexNumber[m].key, binnum[jndx]) == 0)
                {
                    hexnum[jndx] = hexNumber[m].value;
                    break;
                }
            if (m == 16)
                hexnum[jndx] = '?'; // fallback if malformed input
        }

        hexnum[4] = '\n';

        // Copy 4 hex chars + '\n'
        for (size_t j = 0; j < 5; ++j)
            buffer.str[index * 5 + j] = hexnum[j];
    }

    buffer.str[lines * 5] = '\0';
    return buffer;
}