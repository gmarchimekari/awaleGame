#include <game.h>
#include <string.h>

void initializeGame(Game* game, Client* p1, Client* p2) {
    game->p1 = p1;
    game->p2 = p2;
    game->scoreP1 = 0;
    game->scoreP2 = 0;
    game->winner = NULL;
    game->start = time(NULL);
    game->end = 0; // indique que la partie n'est pas terminee  
    game->board = (AwaleBoard*)malloc(sizeof(AwaleBoard));
    initAwaleBoard(game->board);
}

void endGame(Game* game) {
    game->end = time(NULL);
    if(game->scoreP1 > game->scoreP2) {
        game->winner = game->p1;
    } else if(game->scoreP1 < game->scoreP2) {
        game->winner = game->p2;
    } else { // egalite
        game->winner = NULL;
    }
}

int checkEndGame(Game* game) {
    AwaleBoard* awaleBoard = game->board;
    for(int i = 0; i < 12; i++)
        if(awaleBoard->board[i] != 0) 
            return 0;
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

int updateAwaleBoard(Game* game, int move, const Client* client) {
    AwaleBoard* awaleBoard = game->board;
    int ret = 0; // Code de retour, 0 si le client ne rejoue pas, 1 sinon
    move--; // Pour les index de tableau

    int priseToutesGraines = checkPriseToutesGraines(game, client, move);

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



