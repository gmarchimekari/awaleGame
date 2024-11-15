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
#define LOG 4
#define WAG 5
#define SND 6
#define DYP 7
#define BIO 8
#define PVM 9
#define SVG 10
#define ACT 11
#define RJT 12
#define LFR 13
#define LSF 14

#include "client.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

// Functions added
static void send_message_to_client(const Client client, const char *buffer);
static void send_main_menu(Client client); 
static void display_online_players(const Client* clients, const int actual, const Client c); 
static void display_games();
static void display_client_profile(const Client c);  
static void watch_finished_game(const Client c); 
static int getValue(const char *val);
static void send_friend_request(const Client sender, Client* receiver);
static Client get_client_by_name(const Client* clients, const int actual, const char* name);

/**
@brief fonction pour repondre a une demande d'ami
@param sender joueur qui reponds a la demande
@param receiver joueur qui recoit la reponse
@param reply 1 si le joueur accepte la demande, 0 sinon
 */
static void reply_to_friend_request(Client* sender, Client* receiver, const int reply);

#endif /* guard */
