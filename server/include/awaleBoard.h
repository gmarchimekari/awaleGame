#ifndef AwaleBoard_H
#define AwaleBoard_H

#include <stdio.h>
#include <client.h>
#include <stdlib.h>

/**
@brief structure qui contient les informations d'un plateau de jeu Awale
@AwaleBoard tableau de int qui represente le plateau. Chaque case contient le nombre de graines dans la case
 */
typedef struct AwaleBoard {
    int* board;
} AwaleBoard;

/**
@brief initialise un plateau de jeu
@param awaleBoard plateau a initialiser
 */
void initAwaleBoard(AwaleBoard* awaleBoard);


/**
@brief libere la memoire allouee pour un plateau de jeu
@param awaleBoard plateau a liberer
 */
void freeAwaleBoard(AwaleBoard* awaleBoard);

void awaleBoard_sprint(char* buffer, void* data);

#endif // AwaleBoard_H