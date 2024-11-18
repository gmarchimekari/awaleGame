#include <game.h>
#include <stdio.h>
#include <string.h>

void initializeGame(Game* game, Client* p1, Client* p2) {
    static int ID = 1000; // static variable to generate unique ID
    game->ID = ID++;
    game->p1 = p1;
    game->p2 = p2;
    game->scoreP1 = 0;
    game->scoreP2 = 0;
    game->winner = NULL;
    game->start = 0;
    game->end = 0; // indique que la partie n'est pas terminee  
    game->board = (AwaleBoard*)malloc(sizeof(AwaleBoard));
    game->history = (List*)malloc(sizeof(List));
    game->spectators = (List*)malloc(sizeof(List));
    initAwaleBoard(game->board);
    initList(game->history);
    initList(game->spectators);
}

void endGame(Game* game) {
    game->end = time(NULL);
    game->playerTurn = NULL;
    if(game->scoreP1 > game->scoreP2) {
        game->winner = game->p1;
    } else if(game->scoreP1 < game->scoreP2) {
        game->winner = game->p2;
    } else { // egalite
        game->winner = NULL;
    }
}

void game_start(Game* game) {
    game->start = time(NULL);
}

int checkEndGame(Game* game) {

    if (game->scoreP1 >= 25 || game->scoreP2 >= 25) 
        return 1;
    
}

int checkPriseToutesGraines(Game* game, const Client* client, int move) {
    AwaleBoard* awaleBoard = game->board;
    int opponentStart, opponentEnd;

    if (client == game->p2) {
        opponentStart = 0;
        opponentEnd = 5;
    } else {
        opponentStart = 6;
        opponentEnd = 11;
    }

    // Simulate the move
    int nbSeeds = awaleBoard->board[move];
    int tempBoard[12];
    memcpy(tempBoard, awaleBoard->board, sizeof(tempBoard));
    tempBoard[move] = 0;

    int i = (move + 1) % 12;
    while (nbSeeds > 0) {
        if (i == move) {
            i++;
            i %= 12;
            continue;
        }
        tempBoard[i]++;
        nbSeeds--;
        i++;
        i %= 12;
    }

    int lastPit = (i == 0) ? 11 : i - 1; // Dernier trou où une graine a été placée

    // Vérifier si des points sont capturés
    if (tempBoard[lastPit] == 2 || tempBoard[lastPit] == 3) {
        if (client == game->p1 && lastPit >= 6 && lastPit < 12) {
            tempBoard[lastPit] = 0;
            // Cascade de capture de points
            int indexCascade = lastPit - 1;
            while ((tempBoard[indexCascade] == 2 || tempBoard[indexCascade] == 3) && indexCascade >= 6 && indexCascade < 12) {
                tempBoard[indexCascade] = 0;
                indexCascade--;
            }
        } else if (client == game->p2 && lastPit >= 0 && lastPit < 6) {
            tempBoard[lastPit] = 0;
            // Cascade de capture de points
            int indexCascade = lastPit - 1;
            while ((tempBoard[indexCascade] == 2 || tempBoard[indexCascade] == 3) && indexCascade >= 0 && indexCascade < 6) {
                tempBoard[indexCascade] = 0;
                indexCascade--;
            }
        }
    }

    // Check if all opponent's pits are empty
    for (int j = opponentStart; j <= opponentEnd; j++) {
        if (tempBoard[j] > 0) {
            return 0; // Not all pits are empty
        }
    }

    return 1; // All pits are empty 
}

int simulationFamine(Game* game, const Client* client, int move) {
    AwaleBoard* awaleBoard = game->board;
    int opponentStart, opponentEnd;

    if (client == game->p2) {
        opponentStart = 0;
        opponentEnd = 5;
    } else {
        opponentStart = 6;
        opponentEnd = 11;
    }

    // Simulate the move
    int nbSeeds = awaleBoard->board[move];
    int tempBoard[12];
    memcpy(tempBoard, awaleBoard->board, sizeof(tempBoard));
    tempBoard[move] = 0;

    int i = (move + 1) % 12;
    while (nbSeeds > 0) {
        if (i == move) {
            i++;
            i %= 12;
            continue;
        }
        tempBoard[i]++;
        nbSeeds--;
        i++;
        i %= 12;
    }

    // Check if at least one seed is given to the opponent
    for (int j = opponentStart; j <= opponentEnd; j++) {
        if (tempBoard[j] > 0) {
            return 1; // At least one seed is given to the opponent
        }
    }

    return 0; // No seeds are given to the opponent
}

int checkImpossibleFamineResolution(Game* game, const Client* client) {
    AwaleBoard* awaleBoard = game->board;
    int playerStart, playerEnd;

    if (client == game->p2) {
        playerStart = 6;
        playerEnd = 11;
    } else {
        playerStart = 0;
        playerEnd = 5;
    }

    for (int i = playerStart; i <= playerEnd; i++) {
        if (awaleBoard->board[i] > 0) {
            if (simulationFamine(game, client, i) == 1) {
                return 0; // At least one move can resolve the famine
            }
        }
    }

    return -1; // No move can resolve the famine
}

int updateAwaleBoard(Game* game, int move, const Client* client) {
    AwaleBoard* awaleBoard = game->board;
    int ret = 0; // Code de retour, 0 si le client ne rejoue pas, 1 sinon
    move--; // Pour les index de tableau

    int priseToutesGraines = checkPriseToutesGraines(game, client, move);

    // Gestion Famine
    if (simulationFamine(game, client, move) == 0) {
        if (checkImpossibleFamineResolution(game, client) == -1) {
            printf("Fin par famine - Impossible de donner des graines à l'adversaire\n");
            // Capture all remaining seeds and end the game
            int playerStart = (client == game->p1) ? 6 : 0;
            int playerEnd = (client == game->p1) ? 11 : 5;
            for (int i = playerStart; i <= playerEnd; i++) {
                if (client == game->p1) {
                    game->scoreP1 += awaleBoard->board[i];
                } else {
                    game->scoreP2 += awaleBoard->board[i];
                }
                awaleBoard->board[i] = 0;
            }
            game->end = time(NULL);
            return ret;
        } else {
            printf("Vous devez donner des graines à l'adversaire\n");
            return 1;
        }
    }

 
    // Si le joueur a choisi un coup valide, continuer à distribuer les graines
    int nbSeeds = awaleBoard->board[move];
    awaleBoard->board[move] = 0; // Vider le trou choisi

    int i = (move + 1) % 12;
    while (nbSeeds > 0) {
        if (i == move) { // Passer le trou où les graines ont été prises
            i++;
            i %= 12;
            continue;
        }
        awaleBoard->board[i]++;
        nbSeeds--;
        i++;
        i %= 12;
    }

    int lastPit = (i == 0) ? 11 : i - 1; // Dernier trou où une graine a été placée

    // Vérifier si des points sont capturés
    if ((awaleBoard->board[lastPit] == 2 || awaleBoard->board[lastPit] == 3) && priseToutesGraines == 0) {
        if (client == game->p1 && lastPit >= 6 && lastPit < 12) {
            game->scoreP1 += awaleBoard->board[lastPit];
            awaleBoard->board[lastPit] = 0;
            // Cascade de capture de points
            int indexCascade = lastPit - 1;
            while ((awaleBoard->board[indexCascade] == 2 || awaleBoard->board[indexCascade] == 3) && indexCascade >= 6 && indexCascade < 12) {
                game->scoreP1 += awaleBoard->board[indexCascade];
                awaleBoard->board[indexCascade] = 0;
                indexCascade--;
            }
        } else if (client == game->p2 && lastPit >= 0 && lastPit < 6) {
            game->scoreP2 += awaleBoard->board[lastPit];
            awaleBoard->board[lastPit] = 0;
            // Cascade de capture de points
            int indexCascade = lastPit - 1;
            while ((awaleBoard->board[indexCascade] == 2 || awaleBoard->board[indexCascade] == 3) && indexCascade >= 0 && indexCascade < 6) {
                game->scoreP2 += awaleBoard->board[indexCascade];
                awaleBoard->board[indexCascade] = 0;
                indexCascade--;
            }
        }
    }

    // Retourner le code de retour (si le joueur peut rejouer)
    return ret;
}

void displayAwaleBoard(const Game* game){
    printf("this is a test\n");
    AwaleBoard* awaleBoard = game->board;
    printf("+--6-+--5-+--4-+--3-+--2-+--1-+\n");
    printf("-------------------------------\n");
    for (int i = 5; i >= 0; i--) 
        printf("| %2d ", awaleBoard->board[i]);
    printf("|\tJoueur 1: %s, Score : %d\n", game->p1->nickname, game->scoreP1);

    for (int i = 6; i < 12; i++) 
        printf("| %2d ", awaleBoard->board[i]);
    printf("|\tJoueur 2: %s, Score : %d\n", game->p2->nickname, game->scoreP2);

    printf("-------------------------------\n");
    printf("+--1-+--2-+--3-+--4-+--5-+--6-+\n");
}

void freeGame(void* game) {
    Game* g = (Game*)game;
    freeAwaleBoard(g->board);
    free(g);
}

void printGame(void* game) {
    Game* g = (Game*)game;
    printf("Partie: %s VS %s\n", g->p1->nickname, g->p2->nickname);
    printf("Identifiant: %d\n", g->ID);
    printf("Score: %d - %d\n", g->scoreP1, g->scoreP2);
    if(g->winner != NULL) {
        printf("Gagnant: %s\n", g->winner->nickname);
    } else {
        printf("Egalite\n");
    }
    if(g->end != 0) {
        time_t duration = g->end - g->start;
        printf("Duree de la partie: %ld", duration);
    }
    displayAwaleBoard(g);
}

int game_check_player(const void* game, const void* player) {
    Game* g = (Game*)game;
    Client* p = (Client*)player;
    return strcmp(g->p1->nickname, p->nickname) == 0 || strcmp(g->p2->nickname, p->nickname) == 0;
}

void game_sprint(char* buffer, void* data) {
    Game* g = (Game*)data;
    char temp[256];
    sprintf(temp, "Partie: %s VS %s\nScore: %d - %d\n", g->p1->nickname, g->p2->nickname, g->scoreP1, g->scoreP2);
    strcat(buffer, temp);
    sprintf(temp, "Identifiant: %d\n", g->ID);
    strcat(buffer, temp);
    if(g->winner != NULL) {
        sprintf(temp, "Gagnant: %s\n", g->winner->nickname);
        strcat(buffer, temp);
    } else {
        sprintf(temp, "Egalite\n");
        strcat(buffer, temp);
    }
    if(g->end != 0) {
        time_t duration = g->end - g->start;
        sprintf(temp, "Duree de la partie: %ld", duration);
        strcat(buffer, temp);
    }
    awaleBoard_sprint(buffer, g->board);
}



