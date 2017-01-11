#include "SocketWrapper.h"
#include "ClientCommunication.h"
#include "Mutex.h"
#include "Semaphore.h"

DWORD WINAPI runClientCommunicatrion(LPVOID lpParam)
{
	data_communication *communication = (data_communication *) lpParam;
	while (1) 
	{
		if (receive_from_socket(communication->socket, communication->message)) 
		{
			release_semaphore(communication->IncomingMessageFromServerSemaphore);
			WaitForSingleObject(communication->EngineDoneWithServerMessageSemaphore, INFINITE);
		}
		else 
		{
				//TODO Error
		}
		
	}
}
