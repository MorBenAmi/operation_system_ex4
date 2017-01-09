/*Limor Mendelzburg 308081389
Mor Ben Ami 203607536
Exercise 3*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Engine.h"
#include "UiManager.h"
#include "ClientCommunicationThread.h"
#include "Log.h"
#include "Mutex.h"
#include "Semaphore.h"

void runClientCommunicationThread(data_communication *communication);

void runUiThread(data_ui *ui);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void runClient()
{
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;

	HANDLE sem_ui = NULL;
	HANDLE sem_commnunication = NULL;
	data_ui ui;
	data_communication communication;

	ui.semaphore = &sem_ui;
	communication.semaphore = &sem_commnunication;

	sem_ui = create_semaphore("UserEnteredTextSemaphore");
	sem_commnunication = create_semaphore("IncomingMessageFromServerSemaphore");

	mutexes[0] = sem_ui;
	mutexes[1] = sem_commnunication;

	runUiThread(&ui);
	runClientCommunicationThread(&communication);

	if (mutexes[0] == NULL) 
	{ 
		printf("ERROR\n\n");
	}
	
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

	printf("com: %s", ui.command);

	//todo remove
	getchar();
}

void runClientCommunicationThread(data_communication *communication) 
{
	HANDLE clientCommunicationHandle = NULL;
	clientCommunicationHandle = CreateThread(NULL, 0, WaitForMessage, communication, 0, NULL);
	if(clientCommunicationHandle == NULL)
	{
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void runUiThread(data_ui *data) 
{
	HANDLE uiHandle = NULL;
	uiHandle = CreateThread(NULL, 0, readInputFromUser, data, 0, NULL);
	if(uiHandle == NULL)
	{
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}