#ifndef UI_MANEGER
#define UI_MANEGER
#include <Windows.h>

#define MAX_COMMAND_SIZE 1000 //TODO search for the max sizes

#define MUTEX_NAME_USER_ENTERED "UserEnteredTextMutex"
#define MUTEX_NAME_INCOMING_MESSAGE "IncomingMessageFromServerMutex"

DWORD WINAPI readInputFromUser(LPVOID lpParam);

#endif