#include <stdio.h>
#include <stdlib.h>
#include "ClientCommunicationThread.h"
#include "Mutex.h"
#include "Semaphore.h"

DWORD WINAPI WaitForMessage(LPVOID lpParam)
{
	data_communication *commnunication = (data_communication *) lpParam;

	//ReleaseSemaphore(*commnunication->semaphore, 1, NULL);
}
