/**
 * @file server.h
 * @brief Déclarations des fonctions et structures pour la gestion du serveur de jeu.
 * 
 * Ce fichier contient les déclarations des fonctions et structures nécessaires pour
 * la gestion du serveur de jeu. Il inclut les fonctions pour initialiser et terminer
 * le serveur, gérer les connexions des clients, envoyer et recevoir des messages,
 * et gérer les invitations et demandes d'amis.
 */

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

// Définition des valeurs de l'énumération vocab
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
#define LFG 20
#define LYG 21
#define YFG 22
#define EXS 23
#define WFG 24
#define DPP 25

// Réponses que le client peut envoyer
#define ACCEPT 1
#define REJECT 0

#include "client.h"
typedef struct Game Game; // Pour éviter une dépendance circulaire

/**
 * @brief Initialise le serveur.
 */
static void init(void);

/**
 * @brief Termine le serveur.
 */
static void end(void);

/**
 * @brief Fonction principale de l'application serveur.
 */
static void app(void);

/**
 * @brief Initialise la connexion.
 * 
 * @return Le socket de connexion.
 */
static int init_connection(void);

/**
 * @brief Termine la connexion.
 * 
 * @param sock Le socket à fermer.
 */
static void end_connection(int sock);

/**
 * @brief Lit les données envoyées par un client.
 * 
 * @param sock Le socket du client.
 * @param buffer Le buffer pour stocker les données lues.
 * @return Le nombre d'octets lus.
 */
static int read_client(SOCKET sock, char *buffer);

/**
 * @brief Envoie des données à un client.
 * 
 * @param sock Le socket du client.
 * @param buffer Le buffer contenant les données à envoyer.
 */
static void write_client(SOCKET sock, const char *buffer);

/**
 * @brief Envoie un message à tous les clients.
 * 
 * @param clients La liste des clients.
 * @param client Le client qui envoie le message.
 * @param actual Le nombre actuel de clients.
 * @param buffer Le message à envoyer.
 * @param from_server Indique si le message provient du serveur.
 */
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);

/**
 * @brief Supprime un client de la liste des clients.
 * 
 * @param clients La liste des clients.
 * @param to_remove L'indice du client à supprimer.
 * @param actual Le nombre actuel de clients.
 */
static void remove_client(Client *clients, int to_remove, int *actual);

/**
 * @brief Efface tous les clients de la liste.
 * 
 * @param clients La liste des clients.
 * @param actual Le nombre actuel de clients.
 */
static void clear_clients(Client *clients, int actual);

/**
 * @brief Envoie un message à un client spécifique.
 * 
 * @param sender Le client qui envoie le message.
 * @param buffer Le message à envoyer.
 */
static void send_message_to_client(const Client* sender, const char *buffer);

/**
 * @brief Fonction de gestion des messages pour les noeuds de la liste.
 * 
 * @param data Les données du noeud.
 * @param context Le contexte de la fonction.
 */
static void send_message_to_client_handler(void* data, void* context);

/**
 * @brief Envoie le menu principal à un client.
 * 
 * @param receiver Le client qui reçoit le menu.
 */
static void send_main_menu(const Client* receiver);

/**
 * @brief Affiche les joueurs en ligne.
 * 
 * @param clients La liste des clients.
 * @param actual Le nombre actuel de clients.
 * @param sender Le client qui demande la liste des joueurs en ligne.
 */
static void display_online_players(const Client* clients, const int actual, const Client* sender);

/**
 * @brief Récupère la valeur associée à une chaîne de caractères.
 * 
 * @param val La chaîne de caractères.
 * @return La valeur associée.
 */
static int getValue(const char *val);

/**
 * @brief Envoie une demande d'ami à un autre client.
 * 
 * @param sender Le client qui envoie la demande.
 * @param receiver Le client qui reçoit la demande.
 */
static void send_friend_request(Client* sender, Client* receiver);

/**
 * @brief Récupère un client par son nom.
 * 
 * @param clients La liste des clients.
 * @param actual Le nombre actuel de clients.
 * @param name Le nom du client à rechercher.
 * @return Un pointeur vers le client trouvé, ou NULL si non trouvé.
 */
static Client* get_client_by_name(Client* clients, const int actual, const char* name);

/**
 * @brief Fonction pour répondre à une demande d'ami.
 * 
 * @param sender Joueur qui répond à la demande.
 * @param receiver Joueur qui reçoit la réponse.
 * @param reply 1 si le joueur accepte la demande, 0 sinon.
 */
static void reply_to_friend_request(Client* sender, Client* receiver, const int reply);

/**
 * @brief Envoie une invitation de jeu à un autre client.
 * 
 * @param sender Le client qui envoie l'invitation.
 * @param receiver Le client qui reçoit l'invitation.
 */
static void send_game_invite(Client* sender, Client* receiver);

/**
 * @brief Répond à une invitation de jeu.
 * 
 * @param sender Le client qui répond à l'invitation.
 * @param receiver Le client qui a envoyé l'invitation.
 * @param reply 1 si le client accepte l'invitation, 0 sinon.
 * @param games La liste des jeux en cours.
 * @return Un pointeur vers le jeu créé ou NULL si l'invitation est rejetée.
 */
static Game* reply_to_game_invite(Client* sender, Client* receiver, int reply, List* games);

/**
 * @brief Envoie les commandes de jeu à un client.
 * 
 * @param player Le client qui reçoit les commandes.
 */
static void send_game_commands(Client* player);

/**
 * @brief Extrait l'identifiant du jeu à partir d'un buffer.
 * 
 * @param buffer Le buffer contenant l'identifiant du jeu.
 * @return L'identifiant du jeu.
 */
int extract_game_id(const char* buffer);

/**
 * @brief Quitte un jeu.
 * 
 * @param sender Le client qui quitte le jeu.
 * @param game Le jeu à quitter.
 * @param ongoingGames La liste des jeux en cours.
 * @param finishedGames La liste des jeux terminés.
 */
void exit_game(Client* sender, Game* game, List* ongoingGames, List* finishedGames);

#endif /* SERVER_H */