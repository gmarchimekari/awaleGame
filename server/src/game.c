#include <game.h>

void initGame(Game* game, const Player* p1, const Player* p2) {
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

int updateAwaleBoard(AwaleBoard* awaleBoard, const Player* player, int move) {
    int idPlayer;
    
    // Determine the player (either player 1 or player 2)
    if (comparePlayers(player, game->p1)) {
        idPlayer = 0;  // Player 1 uses row 0
    } else if (comparePlayers(player, game->p2)) {
        idPlayer = 1;  // Player 2 uses row 1
    } else {
        printf("Player not found \n");
        return -1;
    }

    // Get the number of seeds in the chosen pit
    int nbSeeds = awaleBoard->board[idPlayer][move];

    // Empty the chosen pit
    awaleBoard->board[idPlayer][move] = 0;

    // Distribute the seeds
    int i;
    int nextPos = move;  // Start at the chosen pit
    for (i = 1; i <= nbSeeds; i++) {
        // Move to the next position (clockwise)
        nextPos = (nextPos + 1) % 12;  // Wrap around if necessary

        // If the next position is within the current player's row (0-5), place the seed
        if (nextPos < 6) {
            awaleBoard->board[idPlayer][nextPos]++;
        } else {
            // If the next position is in the opponent's row (6-11), place the seed there
            int opponentId = (idPlayer == 0) ? 1 : 0;  // Switch player
            awaleBoard->board[opponentId][nextPos - 6]++;
        }
    }

    // Check if the last seed landed in a scoring position (positions 5 or 11)
    if (nextPos == 5 || nextPos == 11) {
        return 1;  // Scoring move
    }

    return 0;  // Non-scoring move
}

int isValidMove(const AwaleBoard* awaleBoard, const Player* player, int move) {
    if(player == player1) {
        return awaleBoard->board[move] != 0;
    } else {
        return awaleBoard->board[move] != 0;
    }
}

void freeGame(void* game) {
    Game* g = (Game*)game;
    freeAwaleBoard(g->board);
    free(g->board);
}

void printGame(void* game) {
    Game* g = (Game*)game;
    printf("Partie: %s VS %s\n", g->p1->name, g->p2->name);
    printf("Score: %d - %d\n", g->scoreP1, g->scoreP2);
    if(g->winner != NULL) {
        printf("Gagnant: %s\n", g->winner->name);
    } else {
        printf("Egalite\n");
    }
    if(g->end != 0) {
        time_t duration = g->end - g->start;
        printf("Duree de la partie: %s", ctime(&g->duration));
    }
    displayAwaleBoard(g->board);
}



