/**
 * @file list.h
 * @brief Définition des structures et des fonctions pour manipuler une liste chaînée.
 * 
 * Ce fichier contient les définitions des structures Node et List, ainsi que les
 * prototypes des fonctions permettant de créer, manipuler, afficher et libérer
 * une liste chaînée. Chaque noeud de la liste peut contenir des données génériques
 * et des fonctions pour afficher et libérer ces données.
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>

/**
 * @brief Pointeur vers une fonction qui prend un void* en paramètre et ne retourne rien.
 */
typedef void (*handler)(void*);

/**
 * @brief Structure qui contient les informations d'un noeud d'une liste chaînée.
 * 
 * @param data Donnée du noeud.
 * @param next Pointeur vers le noeud suivant.
 * @param print Fonction pour afficher la donnée du noeud.
 * @param free Fonction pour libérer la mémoire allouée pour la donnée du noeud.
 * @param sprint Fonction pour copier les informations du noeud dans un buffer.
 */
typedef struct Node {
    void* data;
    struct Node* next;
    handler print;
    handler free;
    void (*sprint)(char*, void*); // fonction pour copier les informations du noeud dans un buffer
} Node;

/**
 * @brief Structure qui contient les informations d'une liste chaînée.
 * 
 * @param head Pointeur vers le premier noeud de la liste.
 */
typedef struct List {
    Node* head;
} List;

// Prototypes des fonctions

/**
 * @brief Crée une liste chaînée.
 * 
 * @param list Liste à créer.
 */
void list_init(List* list);

/**
 * @brief Insère un noeud dans une liste chaînée.
 * 
 * @param list Liste dans laquelle insérer le noeud.
 * @param data Donnée du noeud.
 * @param free Fonction qui libère la mémoire allouée pour la donnée du noeud.
 * @param print Fonction qui affiche la donnée du noeud.
 * @param sprint Fonction qui copie les informations du noeud dans un buffer.
 */
void list_insert_node(List* list, void* data, handler free, handler print, void (*sprint)(char*, void*));

/**
 * @brief Fonction qui cherche un élément dans la liste chaînée.
 * 
 * @param list Liste dans laquelle chercher.
 * @param data Donnée à chercher.
 * @param compare Fonction qui compare deux données.
 * @return 1 si l'élément est trouvé, 0 sinon.
 */
int list_find_node(List* list, void* data, int (*compare)(const void*, const void*));

/**
 * @brief Récupère un noeud par son identifiant.
 * 
 * @param list Liste dans laquelle chercher.
 * @param id Identifiant à chercher.
 * @param compareID Fonction qui compare deux identifiants.
 * @return Pointeur vers la donnée du noeud si trouvé, NULL sinon.
 */
void* list_get_node_by_id(List* list, void* id, int (*compareID)(const void*, const void*)); 

/**
 * @brief Affiche une liste chaînée.
 * 
 * @param list Liste à afficher.
 */
void list_display(const List* list);

/**
 * @brief Libère la mémoire allouée pour une liste chaînée.
 * 
 * @param list Liste à libérer.
 */
void list_free(List* list);

/**
 * @brief Supprime un noeud d'une liste chaînée.
 * 
 * @param list Liste dans laquelle supprimer le noeud.
 * @param data Donnée du noeud à supprimer.
 * @param compare Fonction qui compare deux données.
 * @return Pointeur vers la donnée du noeud supprimé si trouvé, NULL sinon.
 */
void* list_remove_node(List* list, void* data, int (*compare)(const void*, const void*)); 

/**
 * @brief Copie les informations d'une liste chaînée dans un buffer.
 * 
 * @param buffer Buffer pour stocker les informations.
 * @param list Liste à copier.
 */
void list_sprint(char* buffer, const List* list);

/**
 * @brief Applique une fonction à chaque noeud d'une liste chaînée.
 * 
 * @param list Liste à parcourir.
 * @param context Contexte à passer à la fonction.
 * @param handler Fonction à appliquer à chaque noeud.
 */
void list_handle_nodes(List* list, void* context, void (*handler)(void*, void*));

#endif // LIST_H