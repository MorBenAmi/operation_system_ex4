#include "Semaphore.h"

HANDLE create_semaphore(char* name)
{
	return CreateSemaphore(NULL, 0, 1, (LPCWSTR)name);
}

void release_semaphore(HANDLE semaphore) 
{
	ReleaseSemaphore(semaphore, 1, NULL);
}