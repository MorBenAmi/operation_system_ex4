#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <Windows.h>
#include <stdio.h>
#include "Log.h"
#include "Mutex.h"
#include "MutexConstants.h"
#include "GameConsts.h"
#include "SocketWrapper.h"


typedef struct communication_data
{
	SOCKET socket;
	char message[MAX_COMMAND_LENGTH];
	char username[MAX_USER_NAME_LENGTH];
	char symbol;

	char** all_users;
	SOCKET* all_users_sockets;
} communication_data;

DWORD WINAPI ServerCommunicationThreadStart(LPVOID param);

void StartServerCommunication(communication_data* data);

void HandleIncomingMessage(communication_data* data);

BOOL HandleSendMessage(communication_data* data);
