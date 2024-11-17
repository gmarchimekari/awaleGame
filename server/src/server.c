#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "game.h"
#include "client.h"

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
                  // printf("\n\n ---- DEBUG ---- \n\n"); // BUG
                  // debug(clients, actual); // BUG
                  // printf("\n\n ---- END DEBUG ---- \n\n"); // BUG

                  switch(getValue(val)) {
                     case LOP:
                        display_online_players(clients, actual, sender);
                        break;

                     case APF: {
                        printf("[LOG] Sending friend request from %s to %s\n", sender->nickname, buffer + 4);
                        printf("[DEBUG] before sending the friend request\n"); // BUG
                        // debug(clients, actual); // BUG
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever)
                           send_message_to_client(sender, "Player not found\n");
                        else {
                           send_friend_request(sender, reciever);
                           send_message_to_client(sender, "Friend request sent\n");
                        }

                        printf("[DEBUG] after sending the friend request\n"); // BUG
                        // debug(clients, actual); // BUG
                        break;
                     }
                     case CAP: {
                        printf("[LOG] %s challenging %s\n", sender->nickname, buffer + 4);
                        // checking if the player is online
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever)
                           send_message_to_client(sender, "Player not found\n");
                        else { // player found
                           send_game_invite(sender, reciever);  // TODO to be modified later to save the game
                           send_message_to_client(sender, "Game invite sent\n");
                        }
                        printf("[DEBUG] after sending the game invite\n"); // BUG
                        // debug_game_challenge(clients, actual, games);
                        break;
                     }
                     case LOG:
                        //watchFinishedGame(clients[i]); 
                        break;

                     case WAG:
                        //watchFinishedGame(clients[i]); 
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
                        strcpy(sender->bio, buffer + 4);
                        printf("[LOG] %s bio updated\n", sender->nickname);
                        printf("%s\n", sender->bio);
                        send_message_to_client(sender, "Bio updated\n");
                        break;
                     }
                        break;

                     case PVM:   
                        //displayClientProfile(clients[i]); 
                        break;
                     
                     case SVG:
                        //displayClientProfile(clients[i]); 
                        break;

                     case ACT: {
                        printf("[LOG] %s accepting friend request from %s\n", sender->nickname, buffer + 4);
                        printf("[DEBUG] before accepting the friend request\n"); // BUG
                        // debug(clients, actual); // BUG
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to be a friend
                           if(findNode(sender->friends_requests, reciever, compareClients)) // TODO change the name of the function
                              reply_to_friend_request(sender, reciever, 1);
                           else 
                              send_message_to_client(sender, "No friend request from this player\n");
                        }
                        printf("[DEBUG] after accepting the friend request\n"); // BUG
                        // debug(clients, actual); // BUG
                        break;
                     } 

                     case RJT:
                        //displayClientProfile(clients[i]); 
                        break;

                     case LFR:
                        //displayClientProfile(clients[i]); 
                        break;

                     case LSF:
                        
                        break;

                     case ACC: {
                        printf("[LOG] %s accepting game invite from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to play a game
                           if(findNode(sender->game_invites, reciever, game_check_player))
                              reply_to_game_invite(sender, reciever, 1, games);
                           else 
                              send_message_to_client(sender, "No game invite from this player\n");
                        }
                        printf("[DEBUG] after accepting the game invite\n"); // BUG
                        // debug_game_challenge(clients, actual, games); // BUG
                        break;
                     }
                     case RJC:
                        //displayClientProfile(clients[i]); 
                        break;

                     default:
                        printf("Option invalide\n");
                        break;
                  }
                  printf("\n\n ---- DEBUG ---- \n\n"); // BUG
                  debug_game_challenge(clients, actual, games); // BUG
                  printf("\n\n ---- END DEBUG ---- \n\n"); // BUG
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
   } else if(strcmp(val, "LOG") == 0) {
      return LOG;
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
   } else if(strcmp(val, "ACT") == 0) {
      return ACT;
   } else if(strcmp(val, "RJT") == 0) {
      return RJT;
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
   "[APF] [**player name**] Add a player to your friends list\n" // BUG
   "[CAP] [**player name**] Challenge a player\n" 
   "[LOG] List ongoing games\n"
   "[WAG] [**game id**] Watch a game\n"
   "[SND] [**message**] Chat with online players\n" // DONE 
   "[DYP] Display your profile\n" // DONE
   "[BIO] [**new bio**] Modify your bio\n" // DONE 
   "[PVM] [**on/off**] Turn private mode on/off\n"
   "[SVG] Save next game to watch later\n"
   "[LFR] List friend requests\n"
   "[LSF] List friends\n"
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
   strcat(buffer, "\n[ACT] Accept\t[RJT] Reject : followed by the name of the user\n");
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
      insertNode(sender->friends, reciever, freeClient, printClient);
      insertNode(reciever->friends, sender, freeClient, printClient);
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
   insertNode(reciever->game_invites, game, freeGame, printGame);
   send_message_to_client(reciever, buffer);
}

static void reply_to_game_invite(Client* sender, Client* reciever, int reply, List* games) {
   if(reply) {
      // remove the game invite from the list
      void* data = removeNode(sender->game_invites, reciever->nickname, game_check_player);
      // add the game to the ongoing games of the two players
      insertNode(sender->ongoing_games, data, NULL, printGame);
      insertNode(reciever->ongoing_games, data, NULL, printGame);
      insertNode(games, data, freeGame, printGame);
   }
   else {
      void* data = removeNode(reciever->game_invites, sender->nickname, game_check_player);
      Game* game = (Game*)data;
      freeGame(game); // since the game is not going to be played
      send_message_to_client(reciever, "Game invite rejected\n");
   }
}



int main(int argc, char **argv)
{
   init();

   app();

   end();


   // Client clients[10]; 
   // Client* c1 = malloc(sizeof(Client));
   // initializeClient(c1, "c1", "bio1", 1);
   // Client* c2 = malloc(sizeof(Client));
   // initializeClient(c2, "c2", "bio2", 2);
   // Client* c3 = malloc(sizeof(Client));
   // initializeClient(c3, "c3", "bio3", 3);

   // clients[0] = *c1;
   // clients[1] = *c2;
   // clients[2] = *c3;

   // int actual = 3;

   // // sending the friend request
   // Client *reciever = get_client_by_name(clients, actual, "c2");

   // send_friend_request(c1, reciever);

   // printf("this is the friends requests list of %s\n", c2->nickname);
   // displayList(c2->friends_requests);
   
   // // accepting the friend request
   // reply_to_friend_request(reciever, c1, 1);

   // reciever = get_client_by_name(clients, actual, "c3");
   // send_friend_request(c1, reciever);
   // reply_to_friend_request(reciever, c1, 1);

   // for(int i = 0; i < actual; i++) {
   //    printf("this is the friends requests list of %s\n", clients[i].nickname);
   //    displayList(clients[i].friends_requests);
   //    printf("this is the friends list of %s\n", clients[i].nickname);
   //    displayList(clients[i].friends);
   // }

   return EXIT_SUCCESS;
}

// TODO make the client nickname unique
// TODO make the friend requests unique, dont have multiple friend requests from the same person
// TODO check if the user has a frined request from the sender before accepting the request
// TODO cannot send request to yourself 
// TODO cannot send to a friend that is already in the friends list
// TODO dont let the player send multiple friend requests and game invites (before finising a game) to the same player


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