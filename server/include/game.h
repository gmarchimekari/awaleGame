#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <client.h>
#include <time.h>
#include <awaleBoard.h>

/**
@brief structure qui contient les informations d'une partie
@p1 premier joueur
@p2 deuxieme joueur
@scoreP1 score du premier joueur
@scoreP2 score du deuxieme joueur
@winner joueur gagnant
@start temps de debut de la partie
@end temps de fin de la partie
@board plateau de jeu
@ID identifiant de la partie
@playerTurn joueur qui doit jouer
@history historique des coups joues afin de pouvoir regarder la partie de nouveau
@spectators liste des spectateurs
 */
typedef struct Game {
    Client* p1; 
    Client* p2; 
    int scoreP1; 
    int scoreP2; 
    Client* winner; 
    time_t start;
    time_t end;
    AwaleBoard* board;
    int ID; 
    Client* playerTurn;
    List* history; 
    List* spectators;
} Game;

/**
@brief initialise une partie
@param game partie a initialiser
@param p1 premier joueur
@param p2 deuxieme joueur
*/
void initializeGame(Game* game, Client* p1, Client* p2);

/**
@brief termine une partie
@param game partie a terminer
 */
void endGame(Game* game);

/**
@brief verifie si un joueur peut capturer toutes les graines de l'adversaire
@param game partie en cours
@param client joueur qui joue
@param move coup joue
@return 1 si le joueur peut capturer toutes les graines de l'adversaire, 0 sinon
 */
int checkPriseToutesGraines(Game* game, const Client* client, int move);

/**
@brief met a jour le plateau de jeu apres un coup
@param game contient les informations de la partie et le plateau de jeu
@param move coup joue. represente la case jouee
@param Client joueur qui joue
@return 1 si le joueur rejoue, 0 sinon
 */
int updateAwaleBoard(Game* game, int move, const Client* Client); 

/**
@brief affiche un plateau de jeu
@param Game partie en cours
 */
void displayAwaleBoard(const Game* game);


/**
@brief verifie si la partie est terminee
@param game partie a verifier
@return 1 si la partie est terminee, 0 sinon
 */
int checkEndGame(Game* game);

/** 
@brief libere la memoire allouee pour une partie
@param game partie a liberer
*/
void freeGame(void* game);

/**
@brief affiche une partie
@param game partie a afficher
*/
void printGame(void* game);

/**
@brief fonction pour verifier si le joueur est bien un joueur de la partie
 */
int game_check_player(const void* game, const void* player);

int game_compare_id(const void* game, const void* id);

void game_sprint(char* buffer, void* data);

void awaleBoard_sprint(char* buffer, Game* g);

void game_start(Game* game);

void game_string_sprint(char* buffer, void* data);
#endif // GAME_H