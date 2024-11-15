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

int compareClients(const void* p1, const void* p2) {
    Client *c1 = (Client *)p1;
    Client *c2 = (Client *)p2;
    printf("Comparing %s and %s\n", c1->nickname, c2->nickname); // TODO: remove
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

void client_add_friend_request(Client sender, Client* receiver){
    printf("in the client_add_friend_request\n");
    printf("the sender is %s\n", sender.nickname);
    printf("the receiver is %s\n", receiver->nickname);
    insertNode(receiver->friends_requests, &sender, freeClient, printClient);
    displayList(receiver->friends_requests);
}

void client_get_profile_information(const Client c, char* buffer) {
    strcat(buffer, "Name: ");
    strcat(buffer, c.nickname);
    strcat(buffer, "\nBio: ");
    strcat(buffer, c.bio);
    strcat(buffer, "\n");
}


