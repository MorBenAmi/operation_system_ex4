#ifndef SEMAPHORE_HEADER
#define SEMAPHORE_HEADER
#include <windows.h>

HANDLE CreateSemaphoreSimple(char* name);

void ReleaseSemaphoreSimple(HANDLE semaphore);

#endif