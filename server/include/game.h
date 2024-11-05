#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <player.h>
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
 */
typedef struct Game {
    Player* p1; 
    Player* p2; 
    int scoreP1; 
    int scoreP2; 
    Player* winner; 
    time_t start;
    time_t end;
    AwaleBoard* board;
} Game;

/**
@brief initialise une partie
@param game partie a initialiser
@param p1 premier joueur
@param p2 deuxieme joueur
*/
void initGame(Game* game, const Player* p1, const Player* p2);

/**
@brief termine une partie
@param game partie a terminer
 */
void endGame(Game* game);

/**
@brief met a jour le plateau de jeu apres un coup
@param awaleBoard plateau a mettre a jour
@param player joueur qui a joue
@param move coup joue. represente la case jouee
@return 1 si le joueur rejoue, 0 sinon
 */
int updateAwaleBoard(AwaleBoard* awaleBoard, const Player* player, int move);

/**
@brief verifie si un coup est valide
@param awaleBoard plateau de jeu
@param player joueur qui joue
@param move coup joue
@return 1 si le coup est valide, 0 sinon
 */
int isValidMove(const AwaleBoard* awaleBoard, const Player* player, int move);

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


#endif // GAME_H