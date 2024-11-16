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

    initList(Client->games);
    initList(Client->friends);
    initList(Client->friends_requests);
}

int compareClientsNames(const void* p1, const void* p2) {
    char *c1 = (char *)p1;
    char *c2 = (char *)p2;
    return strcmp(c1, c2) == 0;
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

void client_add_friend_request(Client* receiver, char* sender) {
    insertNode(receiver->friends_requests, sender, NULL, client_print_name);
}

void client_print_name(void *name) {
    char *p = (char *)name;
    printf("Client: %s\n", p);
}

void client_get_profile_information(const Client c, char* buffer) {
    strcat(buffer, "Name: ");
    strcat(buffer, c.nickname);
    strcat(buffer, "\nBio: ");
    strcat(buffer, c.bio);
    strcat(buffer, "\n");
}


