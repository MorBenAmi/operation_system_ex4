#ifndef UI_MANEGER_HEADER
#define UI_MANEGER_HEADER
#include <Windows.h>
#include "GameConsts.h"

DWORD WINAPI RunUiManager(LPVOID lpParam);

void ReadFromClient(char *command);

typedef struct data_ui
{
	HANDLE UserEnteredTextSemaphore;
	HANDLE EngineDoneWithUserMessageSemaphore;
	HANDLE PlayersTurnEvent;
	char command[MAX_COMMAND_LENGTH];
} data_ui;

#endif