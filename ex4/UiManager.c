#include "UiManager.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "Events.h"
void ReadFromClient(char *command);

//Runs the ui thread
DWORD WINAPI RunUiManager(LPVOID lpParam)
{
	data_ui *data = (data_ui *)lpParam;
	InitEvent(&data->PlayersTurnEvent, "PlayersTurnEvent");
	while (1) 
	{
		//Reads an input for the client
		ReadFromClient(data->command);
		//Notifies the engine that a message received
		ReleaseSemaphoreSimple(data->UserEnteredTextSemaphore);
		//Waits for the engine to handle the message
		WaitForSingleObject(data->EngineDoneWithUserMessageSemaphore, INFINITE);
	}
	
	return GetLastError();
}

//Reads a message from the client until '\n'
void ReadFromClient(char *command) 
{
	int index = 0;
	char current_char;
	//Resets the buffer
	memset(command, '\0', MAX_COMMAND_LENGTH);
	while ((current_char = getchar()) != '\n')
	{
		command[index++] = current_char;
	}
	command[index] = '\n';
}
