#include "client.h"
#include <stdlib.h>
#include <string.h>

void client_init(Client *Client, const char *name, const char* bio, SOCKET sock) {
    strcpy(Client->nickname, name);
    strcpy(Client->bio, bio);
    Client->sock = sock;
    Client->private = OFF; // at the beginning, the player is not in private mode
    Client->games = malloc(sizeof(List));
    Client->friends = malloc(sizeof(List));
    Client->friends_requests = malloc(sizeof(List));
    Client->game_invites = malloc(sizeof(List));
    Client->ongoing_games = malloc(sizeof(List));
    Client->finished_games = malloc(sizeof(List));

    list_init(Client->games);
    list_init(Client->friends);
    list_init(Client->friends_requests);
    list_init(Client->game_invites);
    list_init(Client->ongoing_games);
    list_init(Client->finished_games);
}

int client_compare(const void* p1, const void* p2) {
    Client *c1 = (Client *)p1;
    Client *c2 = (Client *)p2;
    
    return strcmp(c1->nickname, c2->nickname) == 0;
}

void client_print(void *c) {
    Client *p = (Client *)c;
    printf("Client %s\n", p->nickname);
}

void client_free(void *c) {
    Client *p = (Client *)c;
    list_free(p->games);
    list_free(p->friends);
    list_free(p->friends_requests);
    list_free(p->game_invites);
    list_free(p->ongoing_games);
    list_free(p->finished_games);
    free(p);
}

void client_add_friend_request(Client* clientAdded, Client* clientAdding) {
    list_insert_node(clientAdded->friends_requests, clientAdding, NULL, client_print, client_sprint);
}

void client_get_profile_information(const Client* c, char* buffer) {
    strcat(buffer, "Name: ");
    strcat(buffer, c->nickname);
    strcat(buffer, "\nBio: ");
    strcat(buffer, c->bio);
    strcat(buffer, "\n");
}

void client_sprint(char* buffer, void* data) {
    Client *c = (Client *)data;
    char temp[256];
    sprintf(temp, "Name: %s\nBio: %s\n", c->nickname, c->bio);
    strcat(buffer, temp);
}

