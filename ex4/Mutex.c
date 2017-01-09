#include "Mutex.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//the file containe all the functions that responsable for all the mutexs// 
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

BOOL lock_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD last_error = GetLastError();

	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)mutex_name);
	if(mutex == NULL)
	{
		printf("failed to get mutex: %s\n",mutex_name);
		if(strcmp(mutex_name,"LogFile") != 0)
			write_log("failed to get mutex: %s\n",mutex_name);
		SetEvent(AllThreadsMustEnd);
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
	printf("unable to gain ownership on mutex: %s\n",mutex_name);
	return FALSE;
}

BOOL unlock_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD last_error = 0;

	last_error = GetLastError();
	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)mutex_name);
	if(mutex == NULL)
	{
		printf("failed to get mutex: %s, Error code: 0x%x\n",mutex_name, GetLastError());
		if(strcmp(mutex_name,"LogFile") != 0)
			write_log("failed to get mutex: %s, Error code: 0x%x\n",mutex_name, GetLastError());
		SetEvent(AllThreadsMustEnd);
		return FALSE;
	}
	SetLastError(last_error);

	return ReleaseMutex(mutex);
}

void close_mutex(char* mutex_name)
{
	HANDLE mutex = NULL;
	DWORD lock_result = 0;
	DWORD last_error = 0;

	last_error = GetLastError();
	mutex = CreateMutex(NULL, FALSE, (LPCWSTR)mutex_name);
	if(mutex == NULL)
		return;
	SetLastError(last_error);

	CloseHandle(mutex);
}

HANDLE open_mutex(char* mutex_name) 
{
	return OpenMutex(SYNCHRONIZE, TRUE, (LPCTSTR)mutex_name);
}