#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "game.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   // list of all the games that were accepted by the players
   List* games = (List*) malloc(sizeof(List)); // will be used to free the games at the end
   initList(games);

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         char name[BUF_SIZE] = {0};
         strncpy(name, buffer, BUF_SIZE - 1);

         Client c; 
         initializeClient(&c, name, "Trying to play some awale and chill around\n", csock);
         clients[actual] = c;
         actual++;
         send_main_menu(&c);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.nickname, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               else
               {
                  Client* sender = &clients[i];
                  Client* reciever = NULL;

                  const char val[] = {buffer[0], buffer[1], buffer[2], '\0'};
                  printf("[LOG] User Option %s\n", val);
                  printf("[LOG] Value from enum %d\n", getValue(val));

                  switch(getValue(val)) {
                     case LOP:
                        display_online_players(clients, actual, sender);
                        break;

                     case APF: {
                        printf("[LOG] Sending friend request from %s to %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever)
                           send_message_to_client(sender, "Player not found\n");
                        else {
                           send_friend_request(sender, reciever);
                           send_message_to_client(sender, "Friend request sent\n");
                        }
                        break;
                     }
                     case CAP: {
                        printf("[LOG] %s challenging %s\n", sender->nickname, buffer + 4);
                        // checking if the player is online
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever)
                           send_message_to_client(sender, "Player not found\n");
                        else { // player found
                           send_game_invite(sender, reciever);  
                           send_message_to_client(sender, "Game invite sent\n");
                        }
                        break;
                     }
                     case LSG:
                        printf("[LOG] List of ongoing games\n");
                        strcpy(buffer, "Ongoing games:\n");
                        sprintList(buffer, games);
                        send_message_to_client(sender, buffer);
                        break;

                     case WAG:
                        printf("[LOG] %s spectating a game\n", sender->nickname);
                        // check if the game exists
                        // check if the player is not in the game
                         
                        break;

                     case SND:  
                        send_message_to_all_clients(clients, *sender, actual, buffer + 4, 0);
                        break;

                     case DYP: {
                        char profile[BUF_SIZE] = {0};

                        printf("profile before %s\n", profile);   
                        client_get_profile_information(sender, profile); 
                        printf("profile after %s\n", profile);
                        send_message_to_client(sender, profile);
                        break;
                     }

                     case BIO: {
                        strncpy(sender->bio, buffer + 4, sizeof(sender->bio) - 1);
                        printf("[LOG] %s bio updated\n", sender->nickname);
                        printf("%s\n", sender->bio);
                        send_message_to_client(sender, "Bio updated\n");
                        break;
                     }
                        break;

                     case PVM: {  
                        printf("[LOG] %s changing his mode\n", sender->nickname); 
                        char mode[15]; // giving extra space for the string to not overflow
                        strcpy(mode, buffer + 4); 
                        if(!strcmp(mode, "on")) {
                           sender->private = ON;
                           send_message_to_client(sender, "Private mode on\n");
                        } else if(!strcmp(mode, "off")) {
                           sender->private = OFF;
                           send_message_to_client(sender, "Private mode off\n");
                        } else {
                           send_message_to_client(sender, "Invalid mode\n");
                        }
                        break;
                     }

                     case SVG:
                        //displayClientProfile(clients[i]); 
                        break;

                     case ACF: {
                        printf("[LOG] %s accepting friend request from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to be a friend
                           if(findNode(sender->friends_requests, reciever, compareClients)) // TODO change the name of the function
                              reply_to_friend_request(sender, reciever, ACCEPT);
                           else 
                              send_message_to_client(sender, "No friend request from this player\n");
                        }
                        break;
                     } 

                     case RJF: {
                        printf("[LOG] %s rejecting friend request from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to be a friend
                           if(findNode(sender->friends_requests, reciever, compareClients)) // TODO change the name of the function
                              reply_to_friend_request(sender, reciever, REJECT);
                           else 
                              send_message_to_client(sender, "No friend request from this player\n");
                        }
                        break;
                     }

                     case LFR:
                        printf("[LOG] Friend requests list of %s\n", sender->nickname);
                        strcpy(buffer, "Friend requests:\n");
                        sprintList(buffer, sender->friends_requests);
                        send_message_to_client(sender, buffer);
                        break;

                     case LSF:
                        printf("[LOG] Friends list of %s\n", sender->nickname);
                        strcpy(buffer, "Friends:\n");
                        sprintList(buffer, sender->friends);
                        send_message_to_client(sender, buffer);
                        break;

                     case ACC: {
                        printf("[LOG] %s accepting game invite from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to play a game
                           if(findNode(sender->game_invites, reciever, game_check_player)) {
                              Game* g = NULL; 
                              g = reply_to_game_invite(sender, reciever, ACCEPT, games);
                              // picking up a random player to start the game between sender and reciever
                              g->playerTurn = (rand() % 2 == 0) ? sender : reciever;
                              sprintf(buffer, "Game started! %s goes first.\n", g->playerTurn->nickname);
                              game_sprint(buffer, g);
                              send_message_to_client(sender, buffer);
                              send_message_to_client(reciever, buffer);

                              // sending the game commands to the rigth player
                              send_game_commands(g->playerTurn); 
                              game_start(g);
                           }
                           else 
                              send_message_to_client(sender, "No game invite from this player\n");
                        }
                        break;
                     }
                     case RJC: {
                        printf("[LOG] %s rejecting game invite from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to play a game
                           if(findNode(sender->game_invites, reciever, game_check_player))
                              reply_to_game_invite(sender, reciever, REJECT, games);
                           else 
                              send_message_to_client(sender, "No game invite from this player\n");
                        }
                        break;
                     } 

                     default:
                        send_message_to_client(sender, "Not a valid command\n");
                        break;
                  }
               }
               break;
            }
         }
      }
   }

   freeList(games);
   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.nickname, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

static int getValue(const char *val) 
{
   if(strcmp(val, "LOP") == 0) {
      return LOP;
   } else if(strcmp(val, "APF") == 0) {
      return APF;
   } else if(strcmp(val, "CAP") == 0) {
      return CAP;
   } else if(strcmp(val, "LSG") == 0) {
      return LSG;
   } else if(strcmp(val, "WAG") == 0) {
      return WAG;
   } else if(strcmp(val, "SND") == 0) {
      return SND;
   } else if(strcmp(val, "DYP") == 0) {
      return DYP;
   } else if(strcmp(val, "BIO") == 0) {
      return BIO;
   } else if(strcmp(val, "PVM") == 0) {
      return PVM;
   } else if(strcmp(val, "SVG") == 0) {
      return SVG;
   } else if(strcmp(val, "ACF") == 0) {
      return ACF;
   } else if(strcmp(val, "RJF") == 0) {
      return RJF;
   } else if(strcmp(val, "LFR") == 0) {
      return LFR;
   } else if(strcmp(val, "LSF") == 0) {
      return LSF;
   } else if(strcmp(val, "ACC") == 0) {
      return ACC;
   } else if(strcmp(val, "RJC") == 0) {
      return RJC;
   } else {
      return -1;
   }
}

static void display_online_players(const Client *clients, const int actual, const Client* sender) {
   char buffer[BUF_SIZE] = {"Online Players:\n\0"};
   for(int i = 0; i < actual; i++) {
      if(clients[i].sock > 0) { 
         char temp[BUF_SIZE];
         strcpy(temp, " - ");
         strcat(temp, clients[i].nickname);
         strcat(temp, "\n");
         strcat(buffer, temp);
      }
   }
   send_message_to_client(sender, buffer);
}

static void send_main_menu(const Client* reciever) {
   char* buffer = "Welcome to the Awale game\n"
   "[LOP] List online players\n" // DONE
   "[APF] [**player name**] Add a player to your friends list\n" // DONE
   "[CAP] [**player name**] Challenge a player\n" // DONE // TODO should also start the game here on the accept, later
   "[LSG] List ongoing games\n" // DONE
   "[WAG] [**game id**] Watch a game\n"
   "[MMG] [**game id**] [**move**] Make a move in a game\n" // TODO put after sending the game, and exit the game
   "[SND] [**message**] Chat with online players\n" // DONE 
   "[DYP] Display your profile\n" // DONE
   "[BIO] [**new bio**] Modify your bio\n" // DONE 
   "[PVM] [**on/off**] Turn private mode on/off\n" // DONE
   "[SVG] Save next game to watch later\n"
   "[LFR] List friend requests\n" // DONE
   "[LSF] List friends\n" // DONE
   "Select your option by entering the command: ";
   send_message_to_client(reciever, buffer);

   // TODO
   // voir pour le ranking des joueurs si le time nous 5alina

}

static void send_message_to_client(const Client* sender, const char *buffer) {
   write_client(sender->sock, buffer);
}

static void send_friend_request(Client* sender, Client* reciever) {
   char buffer[BUF_SIZE];
   strcpy(buffer, "Friend request from ");
   strcat(buffer, sender->nickname);
   strcat(buffer, "\n[ACF] Accept\t[RJF] Reject : followed by the name of the user\n");
   client_add_friend_request(reciever, sender); 
   send_message_to_client(reciever, buffer);
}

static Client* get_client_by_name(Client* clients, const int actual, const char* name) { 
   for(int j = 0; j < actual; j++) {
      if(strcmp(clients[j].nickname, name) == 0) 
         return &clients[j]; 
   }
   return NULL;
}

static void reply_to_friend_request(Client* sender, Client* reciever, const int reply) {

   if(reply) {
      insertNode(sender->friends, reciever, NULL, printClient, client_sprint);
      insertNode(reciever->friends, sender, NULL, printClient, client_sprint);
      send_message_to_client(reciever, "Friend request accepted\n");
   }
   else {
      send_message_to_client(reciever, "Friend request rejected\n");
   }

   // remove the friend request from the list
   removeNode(sender->friends_requests, reciever, compareClients); // TODO fuite de memoire, et nom de la fonction a changer, donc ce qu'on passe
}

static void send_game_invite(Client* sender, Client* reciever) {
   char buffer[BUF_SIZE];
   strcpy(buffer, "Game invite from ");
   strcat(buffer, sender->nickname);
   strcat(buffer, "\n[ACC] Accept Challenge\t[RJC] Reject Challenge : followed by the name of the user\n");

   // creation of the game that will be sent to the client and that will be stored in the game invites
   Game *game = (Game*)malloc(sizeof(Game));
   initializeGame(game, sender, reciever);

   // adding it to the list of game invites of the reciever
   insertNode(reciever->game_invites, game, NULL, printGame, game_sprint);
   send_message_to_client(reciever, buffer);
}

static Game* reply_to_game_invite(Client* sender, Client* reciever, int reply, List* games) {
   Game* game = NULL;
   if(reply) {
      // remove the game invite from the list
      void* data = removeNode(sender->game_invites, reciever, game_check_player);
      // add the game to the ongoing games of the two players
      insertNode(sender->ongoing_games, data, NULL, printGame, game_sprint); 
      insertNode(reciever->ongoing_games, data, NULL, printGame, game_sprint); 
      insertNode(games, data, freeGame, printGame, game_sprint); 
      send_message_to_client(reciever, "Game invite accepted\n");
      game = (Game*)data;
   }
   else {
      void* data = removeNode(sender->game_invites, reciever, game_check_player);
      Game* game = (Game*)data;
      freeGame(game); // since the game is not going to be played
      send_message_to_client(reciever, "Game invite rejected\n");
   }
   return game;
}

static void send_game_commands(Client* player) {
   char buffer[BUF_SIZE];
   strcpy(buffer, "Game commands:\n"
   "[MMG] [**game id**] [**move**] Make a move in a game\n"
   "[EXT] [**game id**] Give up on the game\n"
   "[SPM] [**message**] Send a private message to your opponent\n"
   "Select your option by entering the command: ");
   send_message_to_client(player, buffer);
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

//   Client *p1, *p2; 
//    p1 = (Client*)malloc(sizeof(Client));
//    p2 = (Client*)malloc(sizeof(Client));

//    initializeClient(p1, "yano", "password1", 1);
//    initializeClient(p2, "xGdoubleMx", "password2", 1);

//    Game *game = (Game*) malloc(sizeof(Game));
//    initializeGame(game, p1, p2);
   
//    Client *Client = game->p1;
//    while(!game->end) {
//    int move;
//    displayAwaleBoard(game);
//    printf("C'est a %s de jouer, selectionne ton coup: ", Client->nickname);
   
//    scanf("%d", &move);
//    if(move < 1 || move > 6) {
//       printf("Coup invalide\n");
//       continue;
//    }

//    if(compareClientsNames(game->p1->nickname, Client->nickname)) { // Client 1
//       if(!updateAwaleBoard(game, move, Client))
//             Client = game->p2;
//    } else { // Client 2
//       if(!updateAwaleBoard(game, move + 6, Client))
//             Client = game->p1;  
//    }

//    if(checkEndGame(game)) {
//       endGame(game);
//       printf("Partie terminee\n");
//    }
// }
   return EXIT_SUCCESS;

}

// TODO make the client nickname unique
// TODO make the friend requests unique, dont have multiple friend requests from the same person
// TODO check if the user has a frined request from the sender before accepting the request
// TODO cannot send request to yourself 
// TODO cannot send to a friend that is already in the friends list
// TODO dont let the player send multiple friend requests and game invites (before finising a game) to the same player
// TODO when a client sends a friend request or a challenge and disconnects before getting a reply, the lists of the reciever should be updated
// TODO check that the client doesnt send requests to himself

void debug(Client* clients, int actual) {
   printf("\n[DEBUG FUNCTION START]\n");
   for(int i = 0; i < actual; i++) {
      printf("#################################################\n"); 
      printf("Client %s\n", clients[i].nickname);
      printf("--- LIST OF FRIEND REQUESTS ---\n");
      displayList(clients[i].friends_requests);
      printf("\n--- LIST OF FRIENDS ---\n");
      displayList(clients[i].friends);
      printf("#################################################\n"); 
   }
   printf("\n[DEBUG FUNCTION END]\n");

}

void debug_game_challenge(Client* clients, int actual, List* games) {
   printf("\n[DEBUG FUNCTION START FOR GAME INVITES]\n");
   printf("#################################################\n");
   printf("--- LIST OF GAMES ON THE SERVER ---\n");
   displayList(games);
   printf("#################################################\n");
   for(int i = 0; i < actual; i++) {
      printf("#################################################\n"); 
      printf("Client %s\n", clients[i].nickname);
      printf("--- LIST OF GAME INVITES ---\n");
      displayList(clients[i].game_invites);
      printf("--- LIST OF ONGOING GAME ---\n");
      displayList(clients[i].ongoing_games);
      printf("#################################################\n"); 
   }
   printf("\n[DEBUG FUNCTION END GAME INVITES]\n");

}