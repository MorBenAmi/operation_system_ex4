#ifndef CLIENT_COMMUNICATION_HEADER
#define CLIENT_COMMUNICATION_HEADER
#include "SocketWrapper.h"
#include <Windows.h>
#include "Mutex.h"
#include "Semaphore.h"

typedef struct data_communication
{
	HANDLE IncomingMessageFromServerSemaphore;
	HANDLE EngineDoneWithServerMessageSemaphore;
	SOCKET socket;
	char message[MAX_COMMAND_LENGTH];
	char *username;
	char game_piece;
	int port;
	BOOL communication_error;
} data_communication;

DWORD WINAPI RunClientCommunication(LPVOID lpParam);

#endif