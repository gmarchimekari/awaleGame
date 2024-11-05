#include <player.h>
#include <stdlib.h>
#include <string.h>

void initializePlayer(Player *player, const char *name, const char *password) {
    strcpy(player->nickname, name);
    strcpy(player->password, password);
    player->games = malloc(sizeof(List));
    initList(player->games);
}

int comparePlayers(void *player1, void *player2) {
    Player *p1 = (Player *)player1;
    Player *p2 = (Player *)player2;
    return strcmp(p1->nickname, p2->nickname) == 0;
}

void printPlayer(void *player) {
    Player *p = (Player *)player;
    printf("Player %s\n", p->nickname);
}

void freePlayer(void *player) {
    Player *p = (Player *)player;
    freeList(p->games);
    free(p);
}


