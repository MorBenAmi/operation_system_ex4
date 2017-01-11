#ifndef CLIENT_COMMUNICATION_HEADER
#define CLIENT_COMMUNICATION_HEADER
#include <Windows.h>

#define MAX_COMMAND_SIZE 1000 //TODO search for the max sizes
DWORD WINAPI runClientCommunicatrion(LPVOID lpParam);

typedef struct data_communication
{
	HANDLE IncomingMessageFromServerSemaphore;
	HANDLE EngineDoneWithServerMessageSemaphore;
	char message[MAX_COMMAND_SIZE];
	char *username;
	int port;
} data_communication;

#endif