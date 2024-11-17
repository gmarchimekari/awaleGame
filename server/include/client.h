#ifndef CLIENT_H
#define CLIENT_H

// #define BUF_SIZE 1024
#include <stdio.h>
#include <list.h>

typedef struct Client Client;  
typedef int PRIVATE_MODE; // 0 for public, 1 for private, used for spectating matches

#define ON 1
#define OFF 0

#include "server.h"

// TODO : add ranking
/**
@brief structure qui contient les informations d'un joueur
@nickname pseudo du joueur
@bio biographie du joueur
@mode mode du joueur (public ou prive), utilise pour l'observation des parties
@games liste des parties jouees par le joueur
@friends liste des amis du joueur
@friends_requests liste des demandes d'amis recues par le joueur
@sock socket du joueur
 */
typedef struct Client{
    SOCKET sock;
    char nickname[10];
    char bio[90];
    PRIVATE_MODE private; 
    List* games; 
    List* friends; 
    List* friends_requests;
    List* game_invites; 
    List* ongoing_games;
    List* finished_games;
} Client;

/**
@brief initialise un joueur
@param Client joueur a initialiser
@param name pseudo du joueur
@param bio biographie du joueur 
@param sock socket du joueur    
 */
void initializeClient(Client *Client, const char *name, const char* bio, SOCKET sock); 

/**
@brief affiche un joueur
@param Client joueur a afficher
 */
void printClient(void *Client);

/**
@brief libere la memoire allouee pour un joueur
@param Client joueur a liberer
 */
void freeClient(void *Client);

/**
@brief compare deux joueurs
@param nom du premier joueur
@param nom du deuxieme joueur
@return 1 si les joueurs sont egaux, 0 sinon
 */
int compareClients(const void *p1, const void *p2); // TODO: non doublon nom

/**
@brief fonction pour ajouter une demande d'ami a la liste d'amis
@param clientAdded joueur qui a recu la demande
@param clientAdding joueur qui a envoye la demande
 */
void client_add_friend_request(Client* clientAdded, Client* clientAdding); // TODO remove, can be done directy in the main

void client_get_profile_information(const Client* c, char* buffer);

void client_sprint(char* buffer, void* data);


#endif // CLIENT_H