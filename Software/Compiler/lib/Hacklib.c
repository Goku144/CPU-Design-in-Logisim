#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hacklib.h"

Stack *newStack(void){
    Stack *stack = malloc(sizeof *stack);
    stack->bottom = NULL;
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

Queu *newQueu(void){
    Queu *queu = malloc(sizeof *queu);
    queu->bottom = NULL;
    queu->top = NULL;
    queu->size = 0;
    return queu;
}

void push(Stack *stack, void *data){
    if(!stack){
        fprintf(stderr, "Stack is already gone\n");
        return;
    }
    Block *newBlock = malloc(sizeof *newBlock);
    newBlock->data = data;
    newBlock->next = NULL;
    newBlock->prev = NULL;
    if(stack->bottom == NULL){
        stack->top = stack->bottom = newBlock; 
    }else{
        stack->top->next = newBlock;
        newBlock->prev = stack->top;
        stack->top = newBlock;
    }
    stack->size++;
}

void enqueu(Queu *queu, void *data){
    if(!queu){
        fprintf(stderr, "Queu is already gone\n");
        return;
    }
    Block *newBlock = malloc(sizeof *newBlock);
    newBlock->data = data;
    newBlock->next = NULL;
    newBlock->prev = NULL;
    if(queu->bottom == NULL){
        queu->top = queu->bottom = newBlock; 
    }else{
        queu->top->next = newBlock;
        newBlock->prev = queu->top;
        queu->top = newBlock;
    }
    queu->size++;
}

void *pop(Stack *stack){
    if(!stack){
        fprintf(stderr, "Stack is already gone\n");
        return NULL;
    }
    if(stack->size == 0){
        fprintf(stderr, "Nothing else to pop!");
        return NULL;
    }
    void *data = stack->top->data;
    if(stack->size == 1){
        free(stack->top);
        stack->top = stack->bottom = NULL;
    }else{
        Block *newBlock = stack->top->prev;
        stack->top->prev = newBlock->next = NULL;
        free(stack->top);
        stack->top = newBlock;
    }
    stack->size--;
    return data;
}

void *dequeu(Queu *queu){
    if(!queu){
        fprintf(stderr, "Queu is already gone\n");
        return NULL;
    }
    if(queu->size == 0){
        fprintf(stderr, "Nothing else to pop!");
        return NULL;
    }
    void *data = queu->bottom->data;
    if(queu->size == 1){
        free(queu->bottom);
        queu->top = queu->bottom = NULL;
    }else{
        Block *newBlock = queu->bottom->next;
        queu->bottom->next = newBlock->prev = NULL;
        free(queu->bottom);
        queu->bottom = newBlock;
    }
    queu->size--;
    return data;
}

void destroyStack(Stack **stack) //double pointer to have the adress point on stack to make stack point to null
{   
    if(!stack || !*stack){
        fprintf(stderr, "Stack is already gone\n");
        return;
    }
    Stack *s = *stack;
    Block *cur = s->bottom;
    Block *next;
    while (cur) 
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
    free(s);
    *stack = NULL;
}

void destroyQueu(Queu **queu) //double pointer to have the adress point on queu to make queu point to null
{   
    if(!queu || !*queu){
        fprintf(stderr, "Queu is already gone\n");
        return;
    }
    Queu *s = *queu;
    Block *cur = s->bottom;
    Block *next;
    while (cur) 
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
    free(s);
    *queu = NULL;
}