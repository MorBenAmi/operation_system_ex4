
#include <stdio.h>
#include "Log.h"
#include "SocketWrapper.h"

#define MaxUserNameLength 31 // including \n
#define NumOfPlayers 4
#define MaxServerWelcomeMessageLength 100 //todo: change it to the right length!

void start_server(int port);

BOOL WaitForPlayers(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers]);

void CloseConnections(SOCKET user_sockets[NumOfPlayers]);