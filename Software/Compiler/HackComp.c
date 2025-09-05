#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Hacklib.h>


int main(void)
{
    printf("/***** Queu *****/\n");

    Queu *queu = newQueu();
    enqueu(queu, "tst1\n");
    enqueu(queu, "tst2\n");
    enqueu(queu, "tst3\n");
    while (queu->size)
    {
        printf("%s", (char*)dequeu(queu));
    }
    destroyQueu(&queu);
    destroyQueu(&queu);

    printf("/***** Stack *****/\n");

    Stack *stack = newStack();
    push(stack, "tst1\n");
    push(stack, "tst2\n");
    push(stack, "tst3\n");
    while (stack->size)
    {
        printf("%s", (char*)pop(stack));
    }
    destroyStack(&stack);
    destroyStack(&stack);
    return 0;
}