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
//this function first convert the num to string and then uses the function "lock_mutex" to lock the mutex//
BOOL lock_mutex_num(unsigned long int mutex_num)
{
	BOOL result;
	char* mutex_name;
	int n = _snprintf(NULL,0,"%ul", mutex_num);
	mutex_name = (char*)malloc(sizeof(char)*n+1);
	if(mutex_name==NULL)
	{
		printf("Failed to malloc mutex name. Error code: 0x%x\n", GetLastError());
		write_log("Failed to malloc mutex name. Error code: 0x%x\n", GetLastError());
		return FALSE;
	}
	_snprintf(mutex_name, n+1, "%ul", mutex_num);
	result = lock_mutex(mutex_name);
	free(mutex_name);
	return result;
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

BOOL unlock_mutex_num(unsigned long int mutex_num)
{
	BOOL result;
	char* mutex_name;
	int n = _snprintf(NULL,0,"%ul", mutex_num);
	mutex_name = (char*)malloc(sizeof(char)*n+1);
	if(mutex_name==NULL)
	{
		printf("Failed to malloc mutex name. Error code: 0x%x", GetLastError());
		write_log("Failed to malloc mutex name. Error code: 0x%x", GetLastError());
		return FALSE;
	}
	_snprintf(mutex_name, n+1, "%ul", mutex_num);
	result = unlock_mutex(mutex_name);
	free(mutex_name);
	return result;
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

void close_mutex_num(unsigned long int mutex_num)
{
	BOOL result;
	char* mutex_name;
	int n = _snprintf(NULL,0,"%ul", mutex_num);
	mutex_name = (char*)malloc(sizeof(char)*n+1);
	if(mutex_name==NULL)
	{
		printf("Failed to malloc mutex name. Error code: 0x%x", GetLastError());
		return;
	}
	_snprintf(mutex_name, n+1, "%ul", mutex_num);
	close_mutex(mutex_name);
	free(mutex_name);
}