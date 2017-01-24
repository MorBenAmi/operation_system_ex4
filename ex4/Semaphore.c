#include "Semaphore.h"

HANDLE CreateSemaphoreSimple(char* name)
{
	char unique_semaphore_name[MAX_SEMAPHORE_LENGTH];

	memset(unique_semaphore_name, '\0', MAX_SEMAPHORE_LENGTH);
	sprintf(unique_semaphore_name, "%d_%s", GetCurrentProcessId(), name);

	return CreateSemaphore(NULL, 0, 1, (LPCWSTR)unique_semaphore_name);
}

void ReleaseSemaphoreSimple(HANDLE semaphore) 
{
	ReleaseSemaphore(semaphore, 1, NULL);
}