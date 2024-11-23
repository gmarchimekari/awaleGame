/**
 * @file game.h
 * @brief Déclarations des fonctions et structures pour la gestion d'une partie d'Awalé.
 *
 * Ce fichier contient les déclarations des fonctions et structures nécessaires pour
 * initialiser, gérer et terminer une partie d'Awalé. Il inclut également des fonctions
 * pour vérifier les conditions de jeu, mettre à jour le plateau, afficher le plateau,
 * et gérer les joueurs et spectateurs.
 *
 * @date 2024
 */

#ifndef GAME_H
#define GAME_H

#include "client.h"
#include "awaleBoard.h"
#include "list.h"

/**
 * @brief Structure qui contient les informations d'une partie.
 * 
 * @param p1 Premier joueur.
 * @param p2 Deuxième joueur.
 * @param scoreP1 Score du premier joueur.
 * @param scoreP2 Score du deuxième joueur.
 * @param winner Joueur gagnant.
 * @param start Temps de début de la partie.
 * @param end Temps de fin de la partie.
 * @param board Plateau de jeu.
 * @param ID Identifiant de la partie.
 * @param playerTurn Joueur qui doit jouer.
 * @param history Historique des coups joués afin de pouvoir regarder la partie de nouveau.
 * @param spectators Liste des spectateurs.
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
 * @brief Initialise une partie.
 * 
 * @param game Partie à initialiser.
 * @param p1 Premier joueur.
 * @param p2 Deuxième joueur.
 */
void initializeGame(Game* game, Client* p1, Client* p2);

/**
 * @brief Termine une partie.
 * 
 * @param game Partie à terminer.
 */
void endGame(Game* game);

/**
 * @brief Vérifie si un joueur peut capturer toutes les graines de l'adversaire.
 * 
 * @param game Partie en cours.
 * @param client Joueur qui joue.
 * @param move Coup joué.
 * @return 1 si le joueur peut capturer toutes les graines de l'adversaire, 0 sinon.
 */
int checkPriseToutesGraines(Game* game, const Client* client, int move);

/**
 * @brief Met à jour le plateau de jeu après un coup.
 * 
 * @param game Contient les informations de la partie et le plateau de jeu.
 * @param move Coup joué. Représente la case jouée.
 * @param Client Joueur qui joue.
 * @return 1 si le joueur rejoue, 0 sinon.
 */
int updateAwaleBoard(Game* game, int move, const Client* Client); 

/**
 * @brief Affiche un plateau de jeu.
 * 
 * @param game Partie en cours.
 */
void displayAwaleBoard(const Game* game);

/**
 * @brief Vérifie si la partie est terminée.
 * 
 * @param game Partie à vérifier.
 * @return 1 si la partie est terminée, 0 sinon.
 */
int checkEndGame(Game* game);

/** 
 * @brief Libère la mémoire allouée pour une partie.
 * 
 * @param game Partie à libérer.
 */
void freeGame(void* game);

/**
 * @brief Affiche une partie.
 * 
 * @param game Partie à afficher.
 */
void printGame(void* game);

/**
 * @brief Fonction pour vérifier si le joueur est bien un joueur de la partie.
 * 
 * @param game Partie en cours.
 * @param player Joueur à vérifier.
 * @return 1 si le joueur est bien un joueur de la partie, 0 sinon.
 */
int game_check_player(const void* game, const void* player);

/**
 * @brief Compare l'identifiant d'une partie.
 * 
 * @param game Partie en cours.
 * @param id Identifiant à comparer.
 * @return 1 si les identifiants sont égaux, 0 sinon.
 */
int game_compare_id(const void* game, const void* id);

/**
 * @brief Formate les informations d'une partie en chaîne de caractères.
 * 
 * @param buffer Buffer pour stocker la chaîne de caractères.
 * @param data Données de la partie.
 */
void game_sprint(char* buffer, void* data);

/**
 * @brief Formate le plateau de jeu en chaîne de caractères.
 * 
 * @param buffer Buffer pour stocker la chaîne de caractères.
 * @param g Partie en cours.
 */
void awaleBoard_sprint(char* buffer, Game* g);

/**
 * @brief Démarre une partie.
 * 
 * @param game Partie à démarrer.
 */
void game_start(Game* game);

/**
 * @brief Formate les informations d'une partie en chaîne de caractères.
 * 
 * @param buffer Buffer pour stocker la chaîne de caractères.
 * @param data Données de la partie.
 */
void game_string_sprint(char* buffer, void* data);

#endif // GAME_H