#include <List.h>
#include <stdlib.h>

void initList(List* list) {
    list->head = NULL;
}

void insertNode(List* list, void* data, handler free, handler print) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = list->head;
    newNode->free = free;
    newNode->print = print;
    list->head = newNode;
}

void displayList(const List* list) {
    Node* current = list->head;
    while (current != NULL) {
        current->print(current->data);
        current = current->next;
    }
}

void freeList(List* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        if(current->free != NULL)
            current->free(current->data);
        free(current);
        current = next;
    }
    list->head = NULL;
}