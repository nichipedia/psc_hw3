#include <stdio.h> 
#include <stdlib.h> 

struct LinkedListNode {
    int data;
    struct LinkedListNode *next;
};

struct IntLinkedList {
    int size;
    struct LinkedListNode *head;
};

void push(struct IntLinkedList *list, int value) {
    struct LinkedListNode *new_node = (struct LinkedListNode*) malloc(sizeof(struct LinkedListNode));
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

struct IntLinkedList* createIntegerList() {
    struct IntLinkedList *list = (struct IntLinkedList*) malloc(sizeof(struct IntLinkedList));
    list->size = 0;
    list->head = NULL;
    return list;
}

int isInList(struct IntLinkedList *list, int value) {
    struct LinkedListNode *cur = list->head;
    while (cur != NULL) {
        if (cur->data == value) {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}
