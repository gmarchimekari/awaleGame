#include <client.h>
#include <stdlib.h>
#include <string.h>

void initializeClient(Client *Client, const char *name, const char* bio, SOCKET sock) {
    strcpy(Client->nickname, name);
    strcpy(Client->bio, bio);
    Client->sock = sock;
    Client->games = malloc(sizeof(List));
    Client->friends = malloc(sizeof(List));
    Client->friends_requests = malloc(sizeof(List));
    Client->game_invites = malloc(sizeof(List));
    Client->ongoing_games = malloc(sizeof(List));
    Client->finished_games = malloc(sizeof(List));

    initList(Client->games);
    initList(Client->friends);
    initList(Client->friends_requests);
    initList(Client->game_invites);
    initList(Client->ongoing_games);
    initList(Client->finished_games);
}

int compareClients(void* p1, void* p2) {
    Client *c1 = (Client *)p1;
    Client *c2 = (Client *)p2;
    
    return strcmp(c1->nickname, c2->nickname) == 0;
}

void printClient(void *c) {
    Client *p = (Client *)c;
    printf("Client %s\n", p->nickname);
}

void freeClient(void *c) {
    Client *p = (Client *)c;
    freeList(p->games);
    freeList(p->friends);
    freeList(p->friends_requests);
    free(p);
}

void client_add_friend_request(Client* receiver, Client* sender) {
    insertNode(receiver->friends_requests, sender, NULL, client_print_name);
}

void client_print_name(void *c) {
    Client *p = (Client *)c;
    printf("Client: %s\n", p->nickname);
}

void client_get_profile_information(const Client c, char* buffer) {
    strcat(buffer, "Name: ");
    strcat(buffer, c.nickname);
    strcat(buffer, "\nBio: ");
    strcat(buffer, c.bio);
    strcat(buffer, "\n");
}


