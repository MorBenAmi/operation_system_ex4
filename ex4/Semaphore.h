#ifndef SEMAPHORE_HEADER
#define SEMAPHORE_HEADER
#include <windows.h>

HANDLE create_semaphore(char* name);

void release_semaphore(HANDLE *semaphore);

#endif