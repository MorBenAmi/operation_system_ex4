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

	char (*all_users)[MAX_USER_NAME_LENGTH];
	char *all_symbols;
	SOCKET* all_users_sockets;
	HANDLE all_threads_must_end_event;
} communication_data;

DWORD WINAPI ServerCommunicationThreadStart(LPVOID param);

void StartServerCommunication(communication_data* data);

BOOL HandleIncomingMessage(communication_data* data);

BOOL HandleSendMessage(communication_data* data);

BOOL HandlePlayerTurnMessage(communication_data* data);

BOOL HandlePlayerWonMessage(communication_data* data);

BOOL HandlePlayersMessage(communication_data* data);

BOOL ShouldFinishThread(HANDLE all_threads_must_end_event);
