#include "Semaphore.h"

HANDLE CreateSemaphoreSimple(char* name)
{
	return CreateSemaphore(NULL, 0, 1, (LPCWSTR)name);
}

void ReleaseSemaphoreSimple(HANDLE semaphore) 
{
	ReleaseSemaphore(semaphore, 1, NULL);
}