#include <stdio.h>
#include <stdlib.h>
#include "UiManager.h"
#include "Mutex.h"
#include "Semaphore.h"

DWORD WINAPI readInputFromUser(LPVOID lpParam)
{
	char current_char;
	int index = 0;
	data_ui *data = (data_ui *)lpParam;
	memset(data->command, '\0', MAX_COMMAND_SIZE);
	printf("Enter command:\n");
	while ((current_char = getchar()) != '\n')
	{
		data->command[index++] = current_char;
	}
	data->command[index++] = '\n';
	
	release_semaphore(data->semaphore);
	return GetLastError();
}