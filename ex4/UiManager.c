#include <stdio.h>
#include <stdlib.h>
#include "UiManager.h"
#include "Mutex.h"
#include "Semaphore.h"

void readFromClient(char *command);

DWORD WINAPI runUiManager(LPVOID lpParam)
{
	data_ui *data = (data_ui *)lpParam;
	while (1) 
	{
		readFromClient(data->command);
		release_semaphore(data->UserEnteredTextSemaphore);
		WaitForSingleObject(data->EngineDoneWithUserMessageSemaphore, INFINITE);
	}
	
	return GetLastError();
}

void readFromClient(char *command) 
{
	int index = 0;
	char current_char;
	memset(command, '\0', MAX_COMMAND_SIZE);
	while ((current_char = getchar()) != '\n')
	{
		command[index++] = current_char;
	}
	command[index++] = '\n';
}
