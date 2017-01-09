#ifndef CLIENT_COMMUNICATION_HEADER
#define CLIENT_COMMUNICATION_HEADER
#include <Windows.h>

DWORD WINAPI WaitForMessage(LPVOID lpParam);

typedef struct data_communication
{
	HANDLE semaphore;
} data_communication;

#endif