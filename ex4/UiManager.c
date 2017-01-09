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

	printf("Enter command:\n");
	while ((current_char = getchar()) != '\n')
	{
		data->command[index++] = current_char;
	}
	data->command[index++] = '\n';
	data->command[index] = '\0';
	
	release_semaphore(data->semaphore);
	return GetLastError();
}