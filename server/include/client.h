#ifndef CLIENT_H
#define CLIENT_H

// #define BUF_SIZE 1024
#include <stdio.h>
#include <list.h>

typedef struct Client Client;  

#include "server.h"

// TODO : add ranking
/**
@brief structure qui contient les informations d'un joueur
@nickname pseudo du joueur
@password mot de passe du joueur
@games liste des parties jouees par le joueur
@sock socket du joueur
 */
typedef struct Client{
    SOCKET sock;
    char nickname[BUF_SIZE];
    char password[BUF_SIZE]; // TODO : hash password
    List* games; 
} Client;

/**
@brief initialise un joueur
@param Client joueur a initialiser
@param name pseudo du joueur
@param password mot de passe du joueur
 */
void initializeClient(Client *Client, const char *name, const char *password); // TODO

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
@param Client1 premier joueur
@param Client2 deuxieme joueur
@return 1 si les joueurs sont egaux, 0 sinon
 */
int compareClients(const Client *p1, const Client *p2); // TODO: non doublon nom


#endif // CLIENT_H