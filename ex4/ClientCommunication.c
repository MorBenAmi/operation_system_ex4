#include "ClientCommunication.h"

//Runs the client communication thread. This thread reads msgs from the socket.
DWORD WINAPI RunClientCommunication(LPVOID lpParam)
{
	data_communication *communication = (data_communication *) lpParam;
	while (1) 
	{
		memset(communication->message, '\0', MAX_COMMAND_LENGTH);
		if (receive_from_socket(communication->socket, communication->message) == TRUE) 
		{
			ReleaseSemaphoreSimple(communication->IncomingMessageFromServerSemaphore);
			WaitForSingleObject(communication->EngineDoneWithServerMessageSemaphore, INFINITE);
		}
		else 
		{
			SetLastError(WSAGetLastError());
			write_log_and_print("Error while trying to receive data to socket. Error code: 0x%x\n", GetLastError());
			ReleaseSemaphoreSimple(communication->IncomingMessageFromServerSemaphore);
			communication->communication_error = TRUE;
			return GetLastError();
		}
		
	}
	return GetLastError();
}
