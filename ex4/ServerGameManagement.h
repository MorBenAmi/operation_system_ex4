#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <stdio.h>
#include <time.h>
#include "Log.h"
#include "Mutex.h"
#include "MutexConstants.h"
#include "SocketWrapper.h"

#define MaxUserNameLength 31 // including \n
#define NumOfPlayers 4
#define MaxServerWelcomeMessageLength 100 //todo: change it to the right length! including \n
#define MaxPlayerJoinMessageLength 100 //todo: change it to the right length! including \n
#define MaxPlayersMessageLength 100 //todo: change it to the right length! including \n

void start_server(int port);

BOOL wait_for_players(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);

void close_connections(SOCKET user_sockets[NumOfPlayers]);

void write_to_log_order_of_players(char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);

BOOL broadcast_players(SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);

BOOL send_players_to_user(SOCKET user_sock, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);