#ifndef UI_MANEGER_HEADER
#define UI_MANEGER_HEADER
#include <Windows.h>

#define MAX_COMMAND_SIZE 1000 //TODO search for the max sizes

DWORD WINAPI readInputFromUser(LPVOID lpParam);

typedef struct data_ui
{
	HANDLE semaphore;
	char command[MAX_COMMAND_SIZE];
} data_ui;

#endif