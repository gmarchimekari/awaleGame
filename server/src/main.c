#include <stdio.h>
#include <stdlib.h>
#include <game.h>
#include <player.h>

int main() {
    
    

    
    return 0;
}


/**
Player *p1, *p2; 
    p1 = (Player*)malloc(sizeof(Player));
    p2 = (Player*)malloc(sizeof(Player));

    initializePlayer(p1, "yano", "password1");
    initializePlayer(p2, "xGdoubleMx", "password2");

    Game *game = (Game*)malloc(sizeof(Game));
    initializeGame(game, p1, p2);
    
    Player *player = game->p1;
    while(!game->end) {
        int move;
        displayAwaleBoard(game);
        printf("C'est a %s de jouer, selectionne ton coup: ", player->nickname);
        
        scanf("%d", &move);
        if(move < 1 || move > 6) {
            printf("Coup invalide\n");
            continue;
        }

        if(comparePlayers(game->p1, player)) { // player 1
            if(!updateAwaleBoard(game, move, player))
                player = game->p2;
        } else { // player 2
            if(!updateAwaleBoard(game, move + 6, player))
                player = game->p1;  
        }

        if(checkEndGame(game)) {
            endGame(game);
            printf("Partie terminee\n");
        }
    } */