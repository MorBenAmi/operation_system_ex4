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

void handleUserMessage(data_ui *ui);

void handleServerMessage(data_communication *communication);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void runClient(int port, char *username)
{
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;

	data_ui ui;
	data_communication communication;
	communication.port = port;
	communication.username = username;
	runUiThread(&ui);
	runClientCommunicationThread(&communication);
	mutexes[0] = ui.UserEnteredTextSemaphore;
	mutexes[1] = communication.IncomingMessageFromServerSemaphore;

	if (mutexes[0] == NULL || mutexes[1] == NULL) 
	{ 
		printf("ERROR\n\n");
	}
	
	while (1) 
	{
		lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:
				//thread 0 is done
				printf("Wait for object 0 is done\n");
				handleUserMessage(&ui);
				break;
			case WAIT_OBJECT_0 + 1:
				printf("com: %s", communication.message);
				handleServerMessage(&communication);
				break;
				//thread 1 is done
			default:
				printf("result: 0x%x\n", GetLastError());
				//todo exit
				break;
		}
	}

	//todo remove
	getchar();
}

void runClientCommunicationThread(data_communication *communication) 
{
	HANDLE clientCommunicationHandle = NULL;

	communication->IncomingMessageFromServerSemaphore = 
		create_semaphore("IncomingMessageFromServerSemaphore");
	communication->EngineDoneWithServerMessageSemaphore = 
		create_semaphore("EngineDoneWithServerMessageSemaphore");
	//todo check if semaphore creation failed
	clientCommunicationHandle = CreateThread(NULL, 0, runClientCommunicatrion, communication, 0, NULL);
	if(clientCommunicationHandle == NULL)
	{
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void runUiThread(data_ui *ui) 
{
	HANDLE uiHandle = NULL;

	ui->UserEnteredTextSemaphore = 
		create_semaphore("UserEnteredTextSemaphore");
	ui->EngineDoneWithUserMessageSemaphore = 
		create_semaphore("EngineDoneWithUserMessageSemaphore");
	//todo check if semaphore creation failed
	uiHandle = CreateThread(NULL, 0, runUiManager, ui, 0, NULL);
	if(uiHandle == NULL)
	{
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void handleUserMessage(data_ui *ui)
{
	printf("Recieved: %s\n", ui->command);
	Sleep(1000);
	release_semaphore(ui->EngineDoneWithUserMessageSemaphore);
}

void handleServerMessage(data_communication *communication)
{

}
