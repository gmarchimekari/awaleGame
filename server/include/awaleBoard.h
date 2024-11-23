/**
 * @file awaleBoard.h
 * @brief Fichier d'en-tête pour la structure du plateau de jeu Awale et ses fonctions associées.
 *
 * Ce fichier contient la définition de la structure AwaleBoard et les déclarations
 * des fonctions pour initialiser et libérer le plateau de jeu Awale.
 *
 * Le plateau de jeu Awale est représenté comme un tableau d'entiers, où chaque élément
 * du tableau correspond au nombre de graines dans une case spécifique du plateau.
 *
 * @date 2024
 */

#ifndef AWALEBOARD_H
#define AWALEBOARD_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief structure qui contient les informations d'un plateau de jeu Awale
 * @param board tableau de int qui represente le plateau. Chaque case contient le nombre de graines dans la case
 */
typedef struct AwaleBoard {
    int* board;
} AwaleBoard;

/**
 * @brief initialise un plateau de jeu
 * @param awaleBoard plateau a initialiser
 */
void awaleBoard_init(AwaleBoard* awaleBoard);


/**
 * @brief libere la memoire allouee pour un plateau de jeu
 * @param awaleBoard plateau a liberer
 */
void awaleBoard_free(AwaleBoard* awaleBoard);


#endif // AWALEBOARD_H