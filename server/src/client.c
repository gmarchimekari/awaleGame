#include <client.h>
#include <stdlib.h>
#include <string.h>

void initializeClient(Client *Client, const char *name, const char *password) {
    strcpy(Client->nickname, name);
    strcpy(Client->password, password);
    Client->games = malloc(sizeof(List));
    initList(Client->games);
}

int compareClients(const Client* p1, const Client* p2) {
    return strcmp(p1->nickname, p2->nickname) == 0;
}

void printClient(void *c) {
    Client *p = (Client *)c;
    printf("Client %s\n", p->nickname);
}

void freeClient(void *c) {
    Client *p = (Client *)c;
    freeList(p->games);
    free(p);
}


