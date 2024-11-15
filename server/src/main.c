#include <stdio.h>
#include <stdlib.h>
#include <game.h>
#include <client.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
Client *p1, *p2; 
    p1 = (Client*)malloc(sizeof(Client));
    p2 = (Client*)malloc(sizeof(Client));

    initializeClient(p1, "yano", "password1");
    initializeClient(p2, "xGdoubleMx", "password2");

    Game *game = (Game*)malloc(sizeof(Game));
    initializeGame(game, p1, p2);
    
    Client *Client = game->p1;
    while(!game->end) {
        int move;
        displayAwaleBoard(game);
        printf("C'est a %s de jouer, selectionne ton coup: ", Client->nickname);
        
        scanf("%d", &move);
        if(move < 1 || move > 6) {
            printf("Coup invalide\n");
            continue;
        }

        if(compareClients(game->p1, Client)) { // Client 1
            if(!updateAwaleBoard(game, move, Client))
                Client = game->p2;
        } else { // Client 2
            if(!updateAwaleBoard(game, move + 6, Client))
                Client = game->p1;  
        }

        if(checkEndGame(game)) {
            endGame(game);
            printf("Partie terminee\n");
        }
    } */