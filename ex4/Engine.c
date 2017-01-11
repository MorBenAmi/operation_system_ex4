/*Limor Mendelzburg 308081389
Mor Ben Ami 203607536
Exercise 3*/
#include <stdio.h>
#include <stdlib.h>
#include "SocketWrapper.h"
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

void connectToServer(data_communication *communication);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void runClient(int port, char *username)
{
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;

	data_ui ui;
	data_communication communication;
	communication.port = port;
	communication.username = username;
	
	connectToServer(&communication);
	
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

void connectToServer(data_communication *communication) 
{
	char username_message[256];
	if (connect_socket(communication->port, &communication->socket) == TRUE) 
	{
		printf("Connected to server on port %d\n", communication->port); //todo remove and write to log
		memset(username_message, '\0', 256);
		strcat(username_message, "username=");
		strcat(username_message, communication->username);
		strcat(username_message, "\n");
		printf("sending %s\n", username_message);

		if ( write_to_socket(communication->socket, username_message) == FALSE ) 
		{
			printf("Socket error while trying to write data to socket\n");
			//todo error
		}
	}
	else 
	{
		printf("Failed connecting to server on port %d", communication->port);
		//todo exit and free all
	}
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
