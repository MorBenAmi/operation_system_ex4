#ifndef SEMAPHORE_HEADER
#define SEMAPHORE_HEADER
#include <windows.h>
#include <stdio.h>

#define MAX_SEMAPHORE_LENGTH 100

HANDLE CreateSemaphoreSimple(char* name);

void ReleaseSemaphoreSimple(HANDLE semaphore);

#endif