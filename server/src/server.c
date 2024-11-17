#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
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
         send_main_menu(c);
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
                  const char val[] = {buffer[0], buffer[1], buffer[2], '\0'};
                  printf("[LOG] User Option %s\n", val);
                  printf("[LOG] Value from enum %d\n", getValue(val));

                  switch(getValue(val)) {
                     case LOP:
                        display_online_players(clients, actual, client);
                        break;

                     case APF: {
                        printf("[LOG] Sending friend request from %s to %s\n", client.nickname, buffer + 4);
                        Client* receiver = get_client_by_name(clients, actual, buffer + 4);
                        printf("[DEBUG] %s\n", receiver->nickname); // BUG remove
                        if(!receiver)
                           send_message_to_client(client, "Player not found\n");
                        else {
                           send_friend_request(client, receiver);
                           send_message_to_client(client, "Friend request sent\n");
                        }
                        break;
                     }
                     case CAP:
                        //displayClientProfile(clients[i]); 
                        break;

                     case LOG:
                        //watchFinishedGame(clients[i]); 
                        break;

                     case WAG:
                        //watchFinishedGame(clients[i]); 
                        break;

                     case SND:  
                        send_message_to_all_clients(clients, client, actual, buffer + 4, 0);
                        break;

                     case DYP: {
                        char profile[BUF_SIZE] = {0};

                        printf("profile before %s\n", profile);   
                        client_get_profile_information(client, profile); 
                        printf("profile after %s\n", profile);
                        send_message_to_client(client, profile);
                        break;
                     }


                     case BIO: {
                        strcpy(client.bio, buffer + 4);
                        clients[i] = client;
                        printf("[LOG] %s bio updated\n", client.nickname);
                        printf("%s\n", client.bio);
                        send_message_to_client(client, "Bio updated\n");
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
                        printf("[LOG] %s accepting friend request from %s\n", client.nickname, buffer + 4);
                        printf("[DEBUG] Friends requests list of %s\n", client.nickname); // BUG

                        Client* receiver = get_client_by_name(clients, actual, buffer + 4);
                        printf("[DEBUG] %s\n", receiver->nickname); // BUG remove

                        if(!receiver) {
                           send_message_to_client(client, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to be a friend
                           if(findNode(client.friends_requests, receiver->nickname, compareClientsNames)) {
                              printf("[DEBUG] reciever %s, sender %s\n", receiver->nickname, client.nickname); // BUG
                              reply_to_friend_request(&client, receiver, 1);
                              printf("[DEBUG] client friend list %s\n", client.nickname); // BUG
                              displayList(client.friends);
                              printf("[DEBUG] receiver friend list %s\n", receiver->nickname); // BUG
                              displayList(receiver->friends);
                           }
                           else 
                              send_message_to_client(client, "No friend request from this player\n");
                        }
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

                     default:
                        printf("Option invalide\n");
                        break;
                  }
               }
               break;
            }
         }
      }
   }

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
   } else {
      return -1;
   }
}

static void display_online_players(const Client *clients, const int actual, const Client c) {
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
   send_message_to_client(c, buffer);
}

static void send_main_menu(const Client client) {
   char* c = "Welcome to the Awale game\n"
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
   send_message_to_client(client, c);

   // TODO
   // voir pour le ranking des joueurs si le time nous 5alina

}

static void send_message_to_client(const Client client, const char *buffer) {
   write_client(client.sock, buffer);
}

static void send_friend_request(Client sender, Client* receiver) {
   printf("[DEBUG] sender name %s, receiver name %s\n", sender.nickname, receiver->nickname); // BUG
   char buffer[BUF_SIZE];
   strcpy(buffer, "Friend request from ");
   strcat(buffer, sender.nickname);
   strcat(buffer, "\n[ACT] Accept\t[RJT] Reject : followed by the name of the user\n");
   client_add_friend_request(receiver, sender.nickname); // adding the name of the sender to the friend requests list of the receiver
   printf("[DEBUG IN SERVEUR.C] friend request list of %s\n", receiver->nickname); // BUG
   displayList(receiver->friends_requests);
   send_message_to_client(*receiver, buffer);
}

static Client* get_client_by_name(Client* clients, const int actual, const char* name) { 
   for(int j = 0; j < actual; j++) {
      if(strcmp(clients[j].nickname, name) == 0) 
         return &clients[j];
   }
   return NULL;
}

static void reply_to_friend_request(Client* sender, Client* receiver, const int reply) {
   if(reply) {
      insertNode(sender->friends, receiver, freeClient, printClient);
      insertNode(receiver->friends, sender, freeClient, printClient);
      send_message_to_client(*receiver, "Friend request accepted\n");
   }
   else {
      send_message_to_client(*receiver, "Friend request rejected\n");
   }
}




int main(int argc, char **argv)
{
   // init();

   // app();

   // end();

  Client *p1, *p2; 
   p1 = (Client*)malloc(sizeof(Client));
   p2 = (Client*)malloc(sizeof(Client));

   initializeClient(p1, "yano", "password1", 1);
   initializeClient(p2, "xGdoubleMx", "password2", 1);

   Game *game = (Game*) malloc(sizeof(Game));
   initializeGame(game, p1, p2);
   
   Client *Client = game->p1;
   while(!game->end) {
   int move;
   displayAwaleBoard(game);
   printf("C'est a %s de jouer, selectionne ton coup: ", Client->nickname);
   
   scanf("%d", &move);
   if(move < 1 || move > 6) {
      printf("Coup invalide\n");
      continue;
   }

   if(compareClientsNames(game->p1->nickname, Client->nickname)) { // Client 1
      if(!updateAwaleBoard(game, move, Client))
            Client = game->p2;
   } else { // Client 2
      if(!updateAwaleBoard(game, move + 6, Client))
            Client = game->p1;  
   }

   if(checkEndGame(game)) {
      endGame(game);
      printf("Partie terminee\n");
   }
}
   return EXIT_SUCCESS;

}

// TODO make the client nickname unique
// TODO make the friend requests unique, dont have multiple friend requests from the same person
// TODO check if the user has a frined request from the sender before accepting the request
// TODO cannot send request to yourself 
// TODO cannot send to a friend that is already in the friends list