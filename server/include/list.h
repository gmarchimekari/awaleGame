#ifndef LIST_H
#define LIST_H

#include <stdio.h>

/**
@brief pointeur vers une fonction qui prend un void* en parametre et ne retourne rien
 */
typedef void (*handler)(void*);
/**
@brief structure qui contient les informations d'un noeud d'une liste chainee
@data donnee du noeud
@next pointeur vers le noeud suivant
 */
typedef struct Node {
    void* data;
    struct Node* next;
    handler print;
    handler free;
    void (*sprint)(char*, void*); // fonction pour copier les informations du noeud dans un buffer
} Node;


/**
@brief structure qui contient les informations d'une liste chainee
@head pointeur vers le premier noeud de la liste
 */
typedef struct List {
    Node* head;
} List;

// Function prototypes
/**
@brief cree une liste chainee
@param list liste a creer
*/
void initList(List* list);

/**
@brief insere un noeud dans une liste chainee
@param list liste dans laquelle inserer le noeud
@param data donnee du noeud
@param free fonction qui libere la memoire allouee pour la donnee du noeud  
@param print fonction qui affiche la donnee du noeud
@param sprint fonction qui copie les informations du noeud dans un buffer
*/
void insertNode(List* list, void* data, handler free, handler print, void (*sprint)(char*, void*));
/**
@brief fonction qui cherche un element dans la liste chainee
@param list liste dans laquelle chercher
@param data donnee a chercher
@param compare fonction qui compare deux donnees
@return 1 si l'element est trouve, 0 sinon
 */
int findNode(List* list, void* data, int (*compare)(const void*, const void*));

/**
@brief affiche une liste chainee
@param list liste a afficher
 */
void displayList(const List* list);

/**
@brief libere la memoire allouee pour une liste chainee
@param list liste a liberer
 */
void freeList(List* list);

void* removeNode(List* list, void* data, int (*compare)(const void*, const void*)); 

void sprintList(char* buffer, const List* list);


#endif // LIST_H