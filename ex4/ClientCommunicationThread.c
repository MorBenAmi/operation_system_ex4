#include <stdio.h>
#include <stdlib.h>
#include "ClientCommunicationThread.h"
#include "Mutex.h"
#include "MutexConstants.h"

DWORD WINAPI WaitForMessage(LPVOID lpParam)
{
	lock_mutex(MUTEX_NAME_INCOMING_MESSAGE);

	unlock_mutex(MUTEX_NAME_INCOMING_MESSAGE);
}
