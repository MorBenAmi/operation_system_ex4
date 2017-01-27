#include "UiManager.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "Events.h"
void ReadFromClient(char *command);

DWORD WINAPI RunUiManager(LPVOID lpParam)
{
	data_ui *data = (data_ui *)lpParam;
	data->PlayersTurnEvent = InitEvent("PlayersTurnEvent");
	while (1) 
	{
		ReadFromClient(data->command);
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
