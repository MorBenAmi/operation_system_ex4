#include "UiManager.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "Events.h"
#define PLAY "play"
void ReadFromClient(char *command);

DWORD WINAPI RunUiManager(LPVOID lpParam)
{
	data_ui *data = (data_ui *)lpParam;
	InitEvent(&data->PlayersTurnEvent, "PlayersTurnEvent");
	while (1) 
	{
		ReadFromClient(data->command);
		if (strcmp(data->command, PLAY) == 0)
		{
			WaitForSingleObject(data->PlayersTurnEvent, INFINITE);
		}
		ReleaseSemaphoreSimple(data->UserEnteredTextSemaphore);
		WaitForSingleObject(data->EngineDoneWithUserMessageSemaphore, INFINITE);
	}
	
	return GetLastError();
}

void ReadFromClient(char *command) 
{
	int index = 0;
	char current_char;
	memset(command, '\0', MAX_COMMAND_LENGTH);
	while ((current_char = getchar()) != '\n')
	{
		command[index++] = current_char;
	}
}
