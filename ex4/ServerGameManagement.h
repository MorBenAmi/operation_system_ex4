
#include <stdio.h>
#include "Log.h"
#include "SocketWrapper.h"

#define MaxUserNameLength 31 // including \n
#define NumOfPlayers 4
#define MaxServerWelcomeMessageLength 100 //todo: change it to the right length!

void start_server(int port);

BOOL wait_for_players(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);

void close_connections(SOCKET user_sockets[NumOfPlayers]);

void write_to_log_order_of_players(char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);