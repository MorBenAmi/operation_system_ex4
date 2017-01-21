#include "Mutex.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//the file containe all the functions that responsable for all the mutexs// 
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

BOOL lock_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD process_id = 0;
	char unique_mutex_name[MAX_MUTEX_LENGTH];
	DWORD last_error = 0;

	last_error = GetLastError();

	process_id = GetCurrentProcessId();
	memset(unique_mutex_name, '\0', MAX_MUTEX_LENGTH);
	sprintf(unique_mutex_name, "%d_%s", process_id, mutex_name);

	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)unique_mutex_name);
	if(mutex == NULL)
	{
		printf("failed to get mutex: %s\n",unique_mutex_name);
		return FALSE;
	}
	SetLastError(last_error);
	lock_result = WaitForSingleObject(mutex, INFINITE);
	switch(lock_result)
	{
		case WAIT_OBJECT_0:
			return TRUE;
			break;
	}
	printf("unable to gain ownership on mutex: %s\n",unique_mutex_name);
	return FALSE;
}

BOOL unlock_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD process_id = 0;
	char unique_mutex_name[MAX_MUTEX_LENGTH];
	DWORD last_error = 0;

	process_id = GetCurrentProcessId();
	memset(unique_mutex_name, '\0', MAX_MUTEX_LENGTH);
	sprintf(unique_mutex_name, "%d_%s", process_id, mutex_name);

	last_error = GetLastError();
	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)unique_mutex_name);
	if(mutex == NULL)
	{
		printf("failed to get mutex: %s, Error code: 0x%x\n",unique_mutex_name, GetLastError());
		return FALSE;
	}
	SetLastError(last_error);

	return ReleaseMutex(mutex);
}

void close_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD process_id = 0;
	char unique_mutex_name[MAX_MUTEX_LENGTH];
	DWORD last_error = 0;

	process_id = GetCurrentProcessId();
	memset(unique_mutex_name, '\0', MAX_MUTEX_LENGTH);
	sprintf(unique_mutex_name, "%d_%s", process_id, mutex_name);

	last_error = GetLastError();
	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)unique_mutex_name);
	if(mutex == NULL)
		return;
	SetLastError(last_error);

	CloseHandle(mutex);
}