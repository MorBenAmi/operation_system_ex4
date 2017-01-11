#include <stdio.h>
#include <stdlib.h>
#include "SocketWrapper.h"
#include "ClientCommunicationThread.h"
#include "Mutex.h"
#include "Semaphore.h"

//Reading data coming from the server
static DWORD RecvDataThread(SOCKET m_socket, data_communication *communication)
{
	BOOL RecvRes;

	while (1) 
	{
		RecvRes = receive_from_socket(m_socket, communication->message);

		if ( RecvRes == FALSE )
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else
		{
			printf("Recived message from server:%s\n", communication->message);
			release_semaphore(communication->IncomingMessageFromServerSemaphore);
		}
	}

	return 0;
}

DWORD WINAPI runClientCommunicatrion(LPVOID lpParam)
{
	SOCKET server_socket;
	BOOL SendRes;
	char username_message[256];
	data_communication *communication = (data_communication *) lpParam;

	if (connect_socket(communication->port, &server_socket) == TRUE) 
	{
		printf("Connected to server on port %d\n", communication->port); //todo remove and write to log
		memset(username_message, '\0', 256);
		strcat(username_message, "username=");
		strcat(username_message, communication->username);
		strcat(username_message, "\n");
		printf("sending %s\n", username_message);
		SendRes = write_to_socket(server_socket, username_message);
		
		if ( SendRes == FALSE ) 
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		RecvDataThread(server_socket, communication);
	}
	else 
	{
		printf("Failed connecting to server on port %d", communication->port);
		//todo exit and free all
	}
	
	release_semaphore(communication->IncomingMessageFromServerSemaphore);
}
