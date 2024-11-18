#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS     100

#define BUF_SIZE    1024

// definition of enum vocab 
#define LOP 1
#define APF 2
#define CAP 3
#define LSG 4
#define WAG 5
#define SND 6
#define DYP 7
#define BIO 8
#define PVM 9
#define SVG 10
#define ACF 11
#define RJF 12
#define LFR 13
#define LSF 14
#define ACC 15
#define RJC 16
#define MMG 17
#define EXT 18
#define SPM 19

// Replies that the client can send
#define ACCEPT 1
#define REJECT 0

#include "client.h"
typedef struct Game Game; // to not get a circular dependency


static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int ACFual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *ACFual);
static void clear_clients(Client *clients, int ACFual);

// Functions added
static void send_message_to_client(const Client* sender, const char *buffer);
static void send_main_menu(const Client* reciever); 
static void display_online_players(const Client* clients, const int ACFual, const Client* sender); 
static int getValue(const char *val);
static void send_friend_request(Client* sender, Client* reciever);
static Client* get_client_by_name(Client* clients, const int ACFual, const char* name);

/**
@brief fonction pour repondre a une demande d'ami
@param sender joueur qui reponds a la demande
@param reciever joueur qui recoit la reponse
@param reply 1 si le joueur accepte la demande, 0 sinon
 */
static void reply_to_friend_request(Client* sender, Client* reciever, const int reply);

static void send_game_invite(Client* sender, Client* reciever); 

static Game* reply_to_game_invite(Client* sender, Client* reciever, int reply, List* games);

static void send_game_commands(Client* player);

#endif /* guard */
