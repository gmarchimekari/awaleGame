


/**
 * @file client.h
 * @brief Déclaration des structures et fonctions pour la gestion des clients dans le jeu Awale.
 *
 * Ce fichier contient les déclarations des structures et des fonctions utilisées pour gérer les clients
 * dans le jeu Awale. Il inclut des fonctions pour initialiser, afficher, libérer et comparer les clients,
 * ainsi que pour gérer les demandes d'amis et les informations de profil.
 *
 * @date 2023-10-05
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <list.h>

typedef struct Client Client;  
typedef int PRIVATE_MODE; // 0 for public, 1 for private, used for spectating matches

#define ON 1
#define OFF 0

#include "server.h"

/**
 * @brief Structure qui contient les informations d'un joueur.
 *
 * @param nickname Pseudo du joueur.
 * @param bio Biographie du joueur.
 * @param mode Mode du joueur (public ou privé), utilisé pour l'observation des parties.
 * @param games Liste des parties jouées par le joueur.
 * @param friends Liste des amis du joueur.
 * @param friends_requests Liste des demandes d'amis reçues par le joueur.
 * @param sock Socket du joueur.
 * @param game_invites Liste des invitations de parties reçues par le joueur.
 * @param ongoing_games Liste des parties en cours du joueur.
 * @param finished_games Liste des parties terminées du joueur.
 */
typedef struct Client{
    SOCKET sock;
    char nickname[25];
    char bio[125];
    PRIVATE_MODE private; 
    List* games; 
    List* friends; 
    List* friends_requests;
    List* game_invites; 
    List* ongoing_games;
    List* finished_games;
} Client;

/**
 * @brief Initialise un joueur.
 *
 * @param Client Joueur à initialiser.
 * @param name Pseudo du joueur.
 * @param bio Biographie du joueur.
 * @param sock Socket du joueur.
 */
void initializeClient(Client *Client, const char *name, const char* bio, SOCKET sock); 

/**
 * @brief Affiche un joueur.
 *
 * @param Client Joueur à afficher.
 */
void printClient(void *Client);

/**
 * @brief Libère la mémoire allouée pour un joueur.
 *
 * @param Client Joueur à libérer.
 */
void freeClient(void *Client);

/**
 * @brief Compare deux joueurs.
 *
 * @param p1 Nom du premier joueur.
 * @param p2 Nom du deuxième joueur.
 * @return 1 si les joueurs sont égaux, 0 sinon.
 */
int compareClients(const void *p1, const void *p2); 

/**
 * @brief Fonction pour ajouter une demande d'ami à la liste d'amis.
 *
 * @param clientAdded Joueur qui a reçu la demande.
 * @param clientAdding Joueur qui a envoyé la demande.
 */
void client_add_friend_request(Client* clientAdded, Client* clientAdding); 

/**
 * @brief Récupère les informations de profil d'un client.
 *
 * @param c Client dont les informations de profil sont récupérées.
 * @param buffer Buffer où les informations de profil sont stockées.
 */
void client_get_profile_information(const Client* c, char* buffer);

/**
 * @brief Formate les informations d'un client en chaîne de caractères.
 *
 * @param buffer Buffer où les informations formatées sont stockées.
 * @param data Données du client à formater.
 */
void client_sprint(char* buffer, void* data);

#endif // CLIENT_H