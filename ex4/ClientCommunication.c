#include "ClientCommunication.h"


DWORD WINAPI RunClientCommunication(LPVOID lpParam)
{
	data_communication *communication = (data_communication *) lpParam;
	while (1) 
	{
		memset(communication->message, '\0', MAX_COMMAND_SIZE);
		if (receive_from_socket(communication->socket, communication->message)) 
		{
			ReleaseSemaphoreSimple(communication->IncomingMessageFromServerSemaphore);
			WaitForSingleObject(communication->EngineDoneWithServerMessageSemaphore, INFINITE);
		}
		else 
		{
				//TODO Error
		}
		
	}
}
