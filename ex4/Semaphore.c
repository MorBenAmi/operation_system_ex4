#include "Semaphore.h"

HANDLE CreateSemaphoreSimple(char* name)
{
	HANDLE created_semaphore;
	DWORD last_error = 0;
	char unique_semaphore_name[MAX_SEMAPHORE_LENGTH];

	memset(unique_semaphore_name, '\0', MAX_SEMAPHORE_LENGTH);
	sprintf(unique_semaphore_name, "%d_%s", GetCurrentProcessId(), name);

	last_error = GetLastError();

	created_semaphore = CreateSemaphore(NULL, 0, 1, (LPCWSTR)unique_semaphore_name);
	if(created_semaphore == NULL)
		return NULL;

	SetLastError(last_error);
	return created_semaphore;
}

void ReleaseSemaphoreSimple(HANDLE semaphore) 
{
	ReleaseSemaphore(semaphore, 1, NULL);
}