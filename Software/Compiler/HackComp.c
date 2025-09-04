#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"HackComp.h"

LinkedList *createNewLinkedList()
{
    LinkedList *list = malloc(sizeof *list);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

DoubleLinkedList *createNewDoubleLinkedList()
{
    DoubleLinkedList *list = malloc(sizeof *list);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

Stack *createNewStack(){
    return createNewDoubleLinkedList();
}

void addLinkedListNode(LinkedList *list, void *data)
{
    Node *newNode = malloc(sizeof *newNode);
    newNode->data = data;
    newNode->next =NULL;
    if(list->head == NULL)
    {
       list->head = newNode;
       list->tail = list->head;
       list->size++;
       return;
    }
    list->tail->next = newNode;
    list->tail = newNode;
    list->size++;
}

void addDoubleLinkedListNode(DoubleLinkedList *list, void *data)
{
    DoubleNode *newNode = malloc(sizeof *newNode);
    newNode->data = data;
    newNode->next =NULL;
    newNode->prev =NULL;
    if(list->head == NULL)
    {
       list->head = newNode;
       list->tail = list->head;
       list->size++;
       return;
    }
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
    list->size++;
}

void push(Stack *stack, void *data){
    addDoubleLinkedListNode(stack, data);
}

void removeLinkedListNode(LinkedList *list)
{
    if(list->head == NULL)
    {
        fprintf(stderr, "The list is empty\n");
        return;
    }
    if(list->size == 1){
        free(list->head);
        list->head = list->tail = NULL;
        list->size--;
        return;
    }
    Node *curr = list->head;
    while (curr->next->next)
        curr = curr->next;
    free(list->tail);
    curr->next = NULL;
    list->tail= curr;
    list->size--;
}

void *removeDoubleLinkedListNode(DoubleLinkedList *list)
{   
    void *data = NULL;
    if(list->head == NULL)
    {
        fprintf(stderr, "The Space is empty\n");
        return data;
    }
    data = list->tail->data;
    if(list->size == 1){
        free(list->head);
        list->head = list->tail = NULL;
        list->size--;
        return data;
    }
    DoubleNode *old = list->tail;     // save before free
    list->tail = old->prev;           // move tail back (non-NULL since size > 1)
    list->tail->next = NULL;          // terminate list
    free(old);                        // free after relinking
    list->size--;
    return data;
}

void *pop(Stack *stack){
    return removeDoubleLinkedListNode(stack);
}

void freeLinkedList(LinkedList *list)
{
    Node *newNode = list->head;
        while (newNode)
        {
        free(newNode);
        list->head = list->head->next;
        newNode = list->head;
        list->size--;
        }
    free(list);
}

void freeDoubleLinkedList(DoubleLinkedList *list)
{
    if (!list) return;
    DoubleNode *cur = list->head;
    while (cur) {
        DoubleNode *next = cur->next; 
        free(cur);                     
        cur = next;                   
        if (list->size > 0) list->size--;
    }
    list->head = list->tail = NULL;
    free(list);
}


void destroyStack(Stack *stack){
    freeDoubleLinkedList(stack);
}

int main()
{
    return 0;
}