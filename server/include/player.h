#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <list.h>

// TODO : add ranking
/**
@brief structure qui contient les informations d'un joueur
@nickname pseudo du joueur
@password mot de passe du joueur
@games liste des parties jouees par le joueur
 */
typedef struct Player{
    char nickname[50];
    char password[50]; // TODO : hash password
    List* games; 
} Player;

/**
@brief initialise un joueur
@param player joueur a initialiser
@param name pseudo du joueur
@param password mot de passe du joueur
 */
void initializePlayer(Player *player, const char *name, const char *password); // TODO

/**
@brief affiche un joueur
@param player joueur a afficher
 */
void printPlayer(void *player);

/**
@brief libere la memoire allouee pour un joueur
@param player joueur a liberer
 */
void freePlayer(void *player);

/**
@brief compare deux joueurs
@param player1 premier joueur
@param player2 deuxieme joueur
@return 1 si les joueurs sont egaux, 0 sinon
 */
int comparePlayers(const Player *p1, const Player *p2); // TODO: non doublon nom


#endif // PLAYER_H