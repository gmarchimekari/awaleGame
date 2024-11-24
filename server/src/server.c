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

   // generating the random seed
   srand(time(NULL));

   // list of all the games that were accepted by the players
   List* ongoingGames = (List*) malloc(sizeof(List)); // will be used to free the games at the end
   list_init(ongoingGames);

   // list of all the games that were finished
   List* finishedGames = (List*) malloc(sizeof(List)); // will be used to free the games at the end
   list_init(finishedGames);

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

         char id[14]; 
         sprintf(id, "#%05d", rand() % 10000); // randomizing the id between 0 and 9999
         strncat(name, id, 6);
         strncat(name, "\0", 1);
         printf("[LOG] %s connected\n", name);
         Client c; 
         client_init(&c, name, "Trying to play some awale and chill around\n", csock);
         clients[actual] = c;
         actual++;
         
         send_message_to_client(&c, "You are now connected as ");
         send_message_to_client(&c, name);
         send_message_to_client(&c, "\n");

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
                  printf("[LOG] A client disconnected\n");
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.nickname, BUF_SIZE - 1);
               }
               else
               {
                  Client* sender = &clients[i];
                  Client* reciever = NULL;

                  const char val[] = {buffer[0], buffer[1], buffer[2], '\0'};
                  printf("[LOG] %s sent %s\n", sender->nickname, buffer);
                  printf("[LOG] User Option %s\n", val);
                  printf("[LOG] Value from enum %d\n", get_value(val));

                  switch(get_value(val)) {
                     case LOP:
                        display_online_players(clients, actual, sender);
                        break;

                     case APF: {
                        printf("[LOG] Sending friend request from %s to %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever){ 
                           send_message_to_client(sender, "Player not found\n");
                        } else if (client_compare(sender, reciever)) {
                           send_message_to_client(sender, "You cannot add yourself as a friend\n");
                        } else if(list_find_node(sender->friends, reciever, client_compare)) {
                           send_message_to_client(sender, "You are already friends\n");
                        } else if(list_find_node(reciever->friends_requests, sender, client_compare)) {
                           send_message_to_client(sender, "Friend request already sent\n");
                        } else if(list_find_node(sender->friends_requests, reciever, client_compare)) {
                           send_message_to_client(sender, "You already have a friend request from this player\n");
                        } else {
                           send_friend_request(sender, reciever);
                           send_message_to_client(sender, "Friend request sent\n");
                        }
                        break;
                     }
                     case CAP: {
                        printf("[LOG] %s challenging %s\n", sender->nickname, buffer + 4);

                        // checking if the player is online
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                        } else if(client_compare(sender, reciever)) {
                           send_message_to_client(sender, "You cannot challenge yourself\n");
                        } else { 
                           // and we want him to be able to send a challenge to a player that he is playing with
                           send_game_invite(sender, reciever);  
                           send_message_to_client(sender, "Game invite sent\n");
                        }
                        break;
                     }
                     case LSG:
                        printf("[LOG] List of ongoing games on the server\n");
                        strcpy(buffer, "Ongoing games on the server:\n");
                        list_sprint(buffer, ongoingGames);
                        send_message_to_client(sender, buffer);
                        break;
                     
                     case LYG:
                        printf("[LOG] List of the client's ongoing games\n");
                        strcpy(buffer, "Your ongoing games:\n");
                        list_sprint(buffer, sender->ongoing_games);
                        send_message_to_client(sender, buffer);
                        break;

                     case WAG: {
                        printf("[LOG] %s spectating a game\n", sender->nickname);
                        // check if the game exists
                        int gameId = extract_game_id(buffer + 4);
                        Game* g = list_get_node_by_id(ongoingGames, &gameId, game_compare_id);
                        if(!g) {
                           send_message_to_client(sender, "Game not found\n");
                        } else {
                           // check if he is not already spectating
                           if(list_find_node(g->spectators, sender, client_compare)) {
                              send_message_to_client(sender, "You are already spectating this game\n");
                           }
                           // check if the sender is not a player
                           else if(client_compare((void*)g->p1, (void*)sender) || client_compare((void*)g->p2, (void*)sender)) {
                              send_message_to_client(sender, "You are a player in this game, cannot spectate\n");
                           } else {
                              // check if he is a friend of one of the players, private mode wont matter
                              if(list_find_node(g->p1->friends, sender, client_compare) || list_find_node(g->p2->friends, sender, client_compare)) {
                                 list_insert_node(g->spectators, sender, NULL, client_print, client_sprint);
                                 send_message_to_client(sender, "You are now spectating the game\n");
                                 send_message_to_client(g->p1, "A player is spectating the game\n");
                                 send_message_to_client(g->p2, "A player is spectating the game\n");
                              } else {
                                 // check if one of the players has his private mode on
                                 if(g->p1->private == ON || g->p2->private == ON) {
                                    send_message_to_client(sender, "One of the players has his private mode on, cannot spectate\n");
                                 } else {
                                    // none of the players has his private mode on
                                    list_insert_node(g->spectators, sender, NULL, client_print, client_sprint);
                                    send_message_to_client(sender, "You are now spectating the game\n");
                                    send_message_to_client(g->p1, "A player is spectating the game\n");
                                    send_message_to_client(g->p2, "A player is spectating the game\n");
                                 }
                              }
                           }                           
                        }   
                        break;                  
                     }

                     case SND:  
                        send_message_to_all_clients(clients, *sender, actual, buffer + 4, 0);
                        break;

                     case SPM:
                        printf("[LOG] %s sending a private message\n", sender->nickname);
                        // get the name of the reciever
                        char receiver_name[25] = {0};
                        int j = 0;
                        while (buffer[4 + j] != ' ' && buffer[4 + j] != '\0') {
                           receiver_name[j] = buffer[4 + j];
                           j++;
                        }
                        receiver_name[j] = '\0';
                        // check if the reciever is online
                        reciever = get_client_by_name(clients, actual, receiver_name);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else {
                           // send the message
                           send_message_to_client(reciever, "Private message from ");
                           send_message_to_client(reciever, sender->nickname);
                           send_message_to_client(reciever, ": ");
                           send_message_to_client(reciever, buffer + 4 + j + 1);
                        }

                        // send the message
                        break;

                     case DYP: {
                        char profile[BUF_SIZE] = {0};
                        client_get_profile_information(sender, profile); 
                        send_message_to_client(sender, profile);
                        break;
                     }

                     case DPP : {
                        printf("[LOG] %s displaying a player's profile\n", sender->nickname);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else {
                           char profile[BUF_SIZE] = {0};
                           client_get_profile_information(reciever, profile);
                           send_message_to_client(reciever, "A player reviewed your profile\n");
                           send_message_to_client(sender, profile);
                        }
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

                     case ACF: {
                        printf("[LOG] %s accepting friend request from %s\n", sender->nickname, buffer + 4);
                        reciever = get_client_by_name(clients, actual, buffer + 4);
                        if(!reciever) {
                           send_message_to_client(sender, "Player not found\n");
                           break;
                        } else { // player found
                           // check if the player requested to be a friend
                           if(list_find_node(sender->friends_requests, reciever, client_compare)) 
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
                           if(list_find_node(sender->friends_requests, reciever, client_compare)) 
                              reply_to_friend_request(sender, reciever, REJECT);
                           else 
                              send_message_to_client(sender, "No friend request from this player\n");
                        }
                        break;
                     }

                     case LFR:
                        printf("[LOG] Friend requests list of %s\n", sender->nickname);
                        strcpy(buffer, "Friend requests:\n");
                        list_sprint(buffer, sender->friends_requests);
                        send_message_to_client(sender, buffer);
                        break;

                     case LSF:
                        printf("[LOG] Friends list of %s\n", sender->nickname);
                        strcpy(buffer, "Friends:\n");
                        list_sprint(buffer, sender->friends);
                        send_message_to_client(sender, buffer);
                        break;

                     case LFG:
                        printf("[LOG] List of finished games\n");
                        strcpy(buffer, "All finished games on the server:\n");
                        list_sprint(buffer, finishedGames);
                        send_message_to_client(sender, buffer);
                        break;

                     case YFG:
                        printf("[LOG] List the client's finished games\n");
                        strcpy(buffer, "Your finished games:\n");
                        list_sprint(buffer, sender->finished_games);
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
                           if(list_find_node(sender->game_invites, reciever, game_check_player)) {
                              Game* g = NULL; 
                              g = reply_to_game_invite(sender, reciever, ACCEPT, ongoingGames);
                              // picking up a random player to start the game between sender and reciever
                              g->playerTurn = (rand() % 2 == 0) ? sender : reciever;
                              sprintf(buffer, "Game started! %s goes first.\n", g->playerTurn->nickname);
                              game_sprint(buffer, g);
                              send_message_to_client(sender, buffer);
                              send_message_to_client(reciever, buffer);
                              list_handle_nodes(g->spectators, buffer, send_message_to_client_handler); // sends the game to the spectators

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
                           if(list_find_node(sender->game_invites, reciever, game_check_player))
                              reply_to_game_invite(sender, reciever, REJECT, ongoingGames);
                           else 
                              send_message_to_client(sender, "No game invite from this player\n");
                        }
                        break;
                     } 

                     case MMG: {
                        int gameId = extract_game_id(buffer + 6);
                        printf("[LOG] %s making a move in the game number %d\n", sender->nickname, gameId);

                        // check if the game exists
                        Game* g = list_get_node_by_id(sender->ongoing_games, &gameId, game_compare_id);
                        if(!g) {
                           send_message_to_client(sender, "Game not found\n");
                           break;
                        } else {
                           // check if it's the player's turn
                           if(client_compare((void*)g->playerTurn, (void*)sender)) {
                              // check if the input is valid
                              int move = atoi(buffer + 3); 
                              if(move < 0 || move > 6 || buffer[5] != ' ') {
                                 send_message_to_client(sender, "Invalid move, please try again with a valid input\n");
                                 break;
                              }

                              // modifiying the move to be in the range of the player, based on player 1 or 2
                              if(client_compare((void*)g->p2, (void*)sender)) 
                                 move += 6; // we add 6 to the move to be in the range of player 2
                              
                              int playAgain = game_update_board(g, move, sender);
                              // update the game
                              if(!playAgain) {
                                 // change the player's turn
                                 g->playerTurn = (client_compare((void*)g->playerTurn, (void*)g->p1)) ? g->p2 : g->p1; 
                              }
                              // check if the game is finished
                              if(game_check_end(g)) {
                                 game_end(g);
                                 send_message_to_client(g->p1, "Game finished\n");
                                 send_message_to_client(g->p2, "Game finished\n");
                                 list_handle_nodes(g->spectators, "Game finished\n", send_message_to_client_handler);
                                 list_remove_node(g->p1->ongoing_games, &(g->ID), game_compare_id);
                                 Game* finishedGame = list_remove_node(g->p2->ongoing_games, &(g->ID), game_compare_id);
                                 list_insert_node(g->p1->finished_games, finishedGame, NULL, game_print, game_sprint);
                                 list_insert_node(g->p2->finished_games, finishedGame, NULL, game_print, game_sprint);
                                 list_insert_node(finishedGames, finishedGame, game_free, game_print, game_sprint);
                              } else {
                                 if (!playAgain) {
                                 sprintf(buffer, "It's %s's turn\n\n", g->playerTurn->nickname);
                                 send_message_to_client(g->p1, buffer);
                                 send_message_to_client(g->p2, buffer);
                                 } else {
                                    sprintf(buffer, "You can play again\n");
                                    send_message_to_client(sender, buffer);
                                 }
                                 
                                 char gameBuffer[BUF_SIZE];
                                 bzero(gameBuffer, BUF_SIZE);
                                 game_sprint(gameBuffer, g);

                                 // creation of the copy of the string 
                                 char* copy = (char*)malloc(sizeof(char) * BUF_SIZE);
                                 strcpy(copy, gameBuffer); 
                                 list_insert_node(g->history, copy, free, NULL, game_string_sprint); // saving the game state, in strings, no need to get the reference or a copy of the object
                                 list_handle_nodes(g->spectators, gameBuffer, send_message_to_client_handler); // sending the game to the spectators
                                 send_message_to_client(g->playerTurn, gameBuffer);
                                 send_game_commands(g->playerTurn);
                              }
                           } else {
                              send_message_to_client(sender, "Not your turn\n");
                           }
                        }
                        break;
                     }

                     case EXT: {
                        int gameId = extract_game_id(buffer + 4);
                        printf("[LOG] %s giving up on the game number %d\n", sender->nickname, gameId);
                        Game* g = list_get_node_by_id(sender->ongoing_games, &gameId, game_compare_id);
                        if(!g) {
                           send_message_to_client(sender, "Game not found\n");
                           break;
                        } else {
                           // check if it's the player's turn
                           if(client_compare((void*)g->playerTurn, (void*)sender))
                              exit_game(sender, g, ongoingGames, finishedGames);
                           else
                              send_message_to_client(sender, "Not your turn\n");
                        }   
                        break;
                     }
                     
                     case EXS: {
                        printf("[LOG] %s exiting spectating mode\n", sender->nickname);
                        int gameId = extract_game_id(buffer + 4);
                        Game* g = list_get_node_by_id(ongoingGames, &gameId, game_compare_id);
                        if(!g) {
                           send_message_to_client(sender, "Game not found\n");
                        } else {
                           if(list_find_node(g->spectators, sender, client_compare)) {
                              list_remove_node(g->spectators, sender, client_compare);
                              send_message_to_client(sender, "You are no longer spectating the game\n");
                              send_message_to_client(g->p1, "A player is no longer spectating the game\n");
                              send_message_to_client(g->p2, "A player is no longer spectating the game\n");
                           } else {
                              send_message_to_client(sender, "You are not spectating this game\n");
                           }
                        }
                        break;
                     } 

                     case WFG: {
                        int gameId = extract_game_id(buffer + 4);
                        printf("[LOG] %s watching the game number %d\n", sender->nickname, gameId);
                        Game* g = list_get_node_by_id(finishedGames, &gameId, game_compare_id);
                        if(!g) {
                           send_message_to_client(sender, "Game not found\n");
                        } else {
                           char history[BUF_SIZE];
                           bzero(history, BUF_SIZE);
                           list_sprint(history, g->history);  
                           send_message_to_client(sender, history);
                        }
                        break;
                     }

                     case DSC: {
                        printf("[LOG] %s disconnecting\n", sender->nickname);
                        closesocket(sender->sock);
                        remove_client(clients, i, &actual);
                        strncpy(buffer, sender->nickname, BUF_SIZE - 1);
                        break;
                     }

                     default:
                        send_message_to_client(sender, "Not a valid command\n");
                        break;
                  }
                  bzero(buffer, BUF_SIZE);
               }
               break;
            }
         }
      }
   }

   list_free(ongoingGames);
   list_free(finishedGames);
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

static int get_value(const char *val) 
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
   } else if(strcmp(val, "MMG") == 0) {
      return MMG;
   } else if(strcmp(val, "EXT") == 0) {
      return EXT;
   } else if(strcmp(val, "SPM") == 0) {
      return SPM;
   } else if(strcmp(val, "LFG") == 0) {
      return LFG;
   } else if(strcmp(val, "LYG") == 0) {
      return LYG;
   } else if(strcmp(val, "YFG") == 0) {
      return YFG;
   } else if(strcmp(val, "EXS") == 0) {
      return EXS;
   } else if(strcmp(val, "WFG") == 0) {
      return WFG;
   } else if(strcmp(val, "DPP") == 0) {
      return DPP;
   } else if(strcmp(val, "DSC") == 0) {
      return DSC;
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
   "[LSG] List all ongoing games\n" // DONE
   "[LYG] List your ongoing games\n" // DONE
   "[LFG] List all finished games\n" // DONE
   "[YFG] List your finished games\n" // DONE
   "[LFR] List friend requests\n" // DONE
   "[LSF] List friends\n" // DONE
   "[APF] [**player name**] Add a player to your friends list\n" // DONE
   "[CAP] [**player name**] Challenge a player\n" // DONE 
   "[WAG] [**game id**] Watch a game\n" // DONE 
   "[WFG] [**game id**] Watch a game already played\n" // DONE
   "[SND] [**message**] Chat with online players\n" // DONE 
   "[SPM] [**player name**] [**message**] Send a private message to a player online\n" // DONE
   "[DYP] Display your profile\n" // DONE
   "[DPP] [**player name**] Display a player's profile\n" // DONE
   "[BIO] [**new bio**] Modify your bio\n" // DONE 
   "[PVM] [**on/off**] Turn private mode on/off\n" // DONE
   "[DSC] Disconnect\n"; // DONE

   "Select your option by entering the command: ";
   send_message_to_client(reciever, buffer);
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
      list_insert_node(sender->friends, reciever, NULL, client_print, client_sprint);
      list_insert_node(reciever->friends, sender, NULL, client_print, client_sprint);
      send_message_to_client(reciever, "Friend request accepted\n");
   }
   else {
      send_message_to_client(reciever, "Friend request rejected\n");
   }

   // remove the friend request from the list
   list_remove_node(sender->friends_requests, reciever, client_compare); 
}

static void send_game_invite(Client* sender, Client* reciever) {
   char buffer[BUF_SIZE];
   strcpy(buffer, "Game invite from ");
   strcat(buffer, sender->nickname);
   strcat(buffer, "\n[ACC] Accept Challenge\t[RJC] Reject Challenge : followed by the name of the user\n");

   // creation of the game that will be sent to the client and that will be stored in the game invites
   Game *game = (Game*)malloc(sizeof(Game));
   game_init(game, sender, reciever);

   // adding it to the list of game invites of the reciever
   list_insert_node(reciever->game_invites, game, NULL, game_print, game_sprint);
   send_message_to_client(reciever, buffer);
}

static Game* reply_to_game_invite(Client* sender, Client* reciever, int reply, List* games) {
   Game* game = NULL;
   if(reply) {
      // remove the game invite from the list
      void* data = list_remove_node(sender->game_invites, reciever, game_check_player);
      // add the game to the ongoing games of the two players
      list_insert_node(sender->ongoing_games, data, NULL, game_print, game_sprint); 
      list_insert_node(reciever->ongoing_games, data, NULL, game_print, game_sprint); 
      list_insert_node(games, data, game_free, game_print, game_sprint); 
      send_message_to_client(reciever, "Game invite accepted\n");
      game = (Game*)data;
   }
   else {
      void* data = list_remove_node(sender->game_invites, reciever, game_check_player);
      Game* game = (Game*)data;
      game_free(game); // since the game is not going to be played
      send_message_to_client(reciever, "Game invite rejected\n");
   }
   return game;
}

static void send_game_commands(Client* player) {
   char buffer[BUF_SIZE];
   strcpy(buffer, "Game commands:\n"
   "[MMG] [**move**] [**game id**] Make a move in a game\n"
   "[EXT] [**game id**] Give up on the game\n"
   "Select your option by entering the command: ");
   send_message_to_client(player, buffer);
}

int extract_game_id(const char* buffer) {
   int index = 0; // Starting index
   char game_id_str[20]; // Assuming the game ID won't be longer than 19 characters
   int game_id_index = 0;
   int ch;

   while ((ch = buffer[index++]) != ' ' && ch != '\0') {
      game_id_str[game_id_index++] = ch;
   }
   game_id_str[game_id_index] = '\0'; // Null-terminate the string

   // Convert the extracted string to an integer
   int game_id = atoi(game_id_str);
   return game_id;
}

void exit_game(Client* sender, Game* game, List* ongoingGames, List* finishedGames) {
   // remove the game from the ongoing games of the two players
   list_remove_node(game->p2->ongoing_games, &(game->ID), game_compare_id);
   list_remove_node(game->p1->ongoing_games, &(game->ID), game_compare_id);
   list_remove_node(ongoingGames, &(game->ID), game_compare_id);

   list_insert_node(game->p2->finished_games, game, NULL, game_print, game_sprint);
   list_insert_node(game->p1->finished_games, game, NULL, game_print, game_sprint);
   list_insert_node(finishedGames, game, game_free, game_print, game_sprint);

   if(client_compare((void*)game->p1, (void*)sender)) {
      // update of the score, the winner gets the max points
      game->scoreP1 = 0; 
      game->scoreP2= 25;
      send_message_to_client(game->p2, "Game finished, your opponent gave up\n");
   } else {
      // update of the score, the winner gets the max points
      game->scoreP1 = 25;
      game->scoreP2 = 0;
      send_message_to_client(game->p1, "Game finished, your opponent gave up\n");
   }
   list_handle_nodes(game->spectators, "Game finished, a player gave up\n", send_message_to_client_handler);

   // ending the game
   game_end(game);
}

static void send_message_to_client_handler(void* data, void* context){ 
   Client* client = (Client*)data;
   // the context is only a buffer in this case
   char* buffer = (char*)context;
   send_message_to_client(client, "[EXS] [**game id**] Exit spectating mode\n");
   send_message_to_client(client, buffer);
}



int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}