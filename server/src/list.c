#include <List.h>
#include <stdlib.h>
#include "Client.h"

void initList(List* list) {
    list->head = NULL;
}

void insertNode(List* list, void* data, handler free, handler print, void (*sprint)(char*, void*)) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = list->head;
    newNode->free = free;
    newNode->print = print;
    newNode->sprint = sprint;
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

int findNode(List* list, void* data, int (*compare)(const void*, const void*)) {
    Node* current = list->head;
    while (current != NULL) {
        if (compare(current->data, data)) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void* getNodeByID(List* list, void* id, int (*compareID)(const void*, const void*)) {
    Node* current = list->head;
    while (current != NULL) {
        if (compareID(current->data, id)) {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}


// frees the node, but not the data in the node
void* removeNode(List* list, void* data, int (*compare)(const void*, const void*)) {
    Node* current = list->head;
    Node* previous = NULL;
    while (current != NULL) {
        if (compare(current->data, data)) {
            void* removedData = current->data;  // Correctly assign data            
            // Remove the node from the list
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            
            free(current);  // Free the node
            return removedData;  // Return the data
        }
        previous = current;
        current = current->next;
    }
    return NULL; // Data not found
}

void sprintList(char* buffer, const List* list) {
    Node* current = list->head;
    while (current != NULL) {
        if(current->sprint != NULL)
            current->sprint(buffer, current->data);
        else
            printf("[ERROR] No sprint function for the data\n");
        current = current->next;
    }
}

void handleNodes(List* list, void* context ,void (*handler)(void*, void*)) {
    Node* current = list->head;
    while (current != NULL) {
        handler(current->data, context);
        current = current->next;
    }
}