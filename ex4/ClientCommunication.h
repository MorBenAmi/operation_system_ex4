#ifndef CLIENT_COMMUNICATION_HEADER
#define CLIENT_COMMUNICATION_HEADER
#include "SocketWrapper.h"
#include <Windows.h>
#include "Mutex.h"
#include "Semaphore.h"

#define MAX_COMMAND_SIZE 80 //TODO search for the max sizes

typedef struct data_communication
{
	HANDLE IncomingMessageFromServerSemaphore;
	HANDLE EngineDoneWithServerMessageSemaphore;
	SOCKET socket;
	char message[MAX_COMMAND_SIZE];
	char *username;
	char game_piece;
	int port;
} data_communication;
DWORD WINAPI RunClientCommunication(LPVOID lpParam);

#endif