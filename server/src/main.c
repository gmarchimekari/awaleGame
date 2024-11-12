#include <stdio.h>
#include <stdlib.h>
#include <game.h>
#include <player.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void mainMenu() {
    printf("Bienvenue sur le jeu Awale\n");
    printf("1. Se connecter\n");
    printf("2. S'inscrire\n"); 
    printf("3. Quitter\n");
    printf("Selectionnez une option: ");
}


int test(int argc, char** argv) // TODO
{ 
    int sockfd,newsockfd,clilen;
    int pid;
    int choice; 
    struct sockaddr_in cli_addr,serv_addr;
    
    if (argc!=2) {printf ("usage: socket_server port\n");exit(0);}
    
    printf ("server starting...\n");  
    
    /* ouverture du socket */
    sockfd = socket (AF_INET,SOCK_STREAM,0);
    if (sockfd<0) {printf ("impossible d'ouvrir le socket\n");exit(0);}

    /* initialisation des parametres */
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family       = AF_INET;
    serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
    serv_addr.sin_port         = htons(atoi(argv[1]));

    /* effecture le bind */
    if (bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        {printf ("impossible de faire le bind\n");exit(0);}

    /* petit initialisation */
    listen(sockfd,1);
        
    signal(SIGCHLD,SIG_IGN); // TODO
    /* attend la connection d'un client */
    clilen = sizeof (cli_addr);
    while(1) {
        newsockfd = accept (sockfd,(struct sockaddr*) &cli_addr, &clilen);
        pid = fork(); 
        if(pid == 0) {
        // ps fils
        if (newsockfd<0) {printf ("accept error\n"); exit(0);}
        printf ("connection accepted\n");
        while(1) {
            mainMenu(); 
            read(newsockfd, &choice, 1);
            switch(choice) {
                case 1:
                    printf("Se connecter\n");
                    
                    break;
                case 2:
                    printf("S'inscrire\n");
                    break;
                case 3:
                    printf("Quitter\n");
                    break;
                default:
                    printf("Option invalide\n");
                    break;
                }
            }
        }
    }

   /*  attention il s'agit d'une boucle infinie 
    *  le socket nn'est jamais ferme !
    */

   return 1;
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