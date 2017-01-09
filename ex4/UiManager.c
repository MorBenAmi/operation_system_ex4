#include <stdio.h>
#include <stdlib.h>
#include "UiManager.h"
#include "Mutex.h"
#include "MutexConstants.h"

DWORD WINAPI readInputFromUser(LPVOID lpParam)
{
	char current_char;
	int index = 0;
	char *input = (char *)lpParam;

	lock_mutex(MUTEX_NAME_USER_ENTERED);
	lock_mutex(MUTEX_NAME_INCOMING_MESSAGE);
	printf("Enter command:\n");
	while ((current_char = getchar()) != '\n')
	{
		input[index++] = current_char;
	}
	input[index++] = '\n';
	input[index] = '\0';

	unlock_mutex(MUTEX_NAME_USER_ENTERED);
	unlock_mutex(MUTEX_NAME_INCOMING_MESSAGE);
	return GetLastError();
}