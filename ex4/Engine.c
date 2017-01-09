/*Limor Mendelzburg 308081389
Mor Ben Ami 203607536
Exercise 3*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Engine.h"
#include "UiManager.h"
#include "Log.h"
#include "Mutex.h"
#include "MutexConstants.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void runClient()
{
	char command[MAX_COMMAND_SIZE]={NULL};
	HANDLE uiHandle = NULL;
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;

	uiHandle = CreateThread(NULL, 0, readInputFromUser, command, 0, NULL);
	if(uiHandle == NULL)
	{
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
	Sleep(1000); //todo fix. maybe send an event that it is done loading
	mutexes[0] = open_mutex(MUTEX_NAME_USER_ENTERED);
	if (mutexes[0] == NULL) 
	{ 
		printf("ERROR\n\n");
	}
	mutexes[1] = open_mutex(MUTEX_NAME_INCOMING_MESSAGE);
	//todo 
	lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
	switch (lock_result)
	{
		case WAIT_OBJECT_0:
			//thread 0 is done
			printf("Wait for object 0 is done");
			break;
		case WAIT_OBJECT_0 + 1:
			printf("Wait for object 1 is done");
			break;
			//thread 1 is done
		default:
			printf("result: 0x%x\n", GetLastError());
			break;
	}

	printf("com: %s", command);

	//todo remove
	getchar();
}