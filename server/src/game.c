#include <game.h>

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


int updateAwaleBoard(Game* game, int move, const Client* Client) {    
    AwaleBoard* awaleBoard = game->board;
    int ret = 0; // return code, 0 if the Client doesn't play again, 1 otherwise
    move--; // for the array indexes
    // Get the number of seeds in the chosen pit
    int nbSeeds = awaleBoard->board[move];

    // Empty the chosen pit
    awaleBoard->board[move] = 0;

    // Distribute the seeds
    int i = move + 1;
    
    // TODO fix client was object now name
    // if(move + nbSeeds == 12 && compareClients(Client, game->p2) || move + nbSeeds == 6 && compareClients(Client, game->p1)) {
    //     ret = 1; // the Client plays again
    // }

    while(nbSeeds > 0) {
        if(i == 6) {
            game->scoreP1++;
            nbSeeds--; 
        } else if(i == 12) {
            game->scoreP2++;
            nbSeeds--;
            i = 0;
        }
        awaleBoard->board[i]++;
        nbSeeds--;
        i++;  
    }
    return ret;
}

void displayAwaleBoard(const Game* game){
    AwaleBoard* awaleBoard = game->board;
    printf("+--1-+--2-+--3-+--4-+--5-+--6-+\n");
    printf("-------------------------------\n");
    for (int i = 0; i < 6; i++) 
        printf("| %2d ", awaleBoard->board[i]);
    printf("|\tJoueur 1: %s\n", game->p1->nickname);

    for (int i = 11; i > 5 ; i--) 
        printf("| %2d ", awaleBoard->board[i]);
    printf("|\tJoueur 2: %s\n", game->p2->nickname);

    printf("-------------------------------\n");
    printf("+--6-+--5-+--4-+--3-+--2-+--1-+\n");
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



