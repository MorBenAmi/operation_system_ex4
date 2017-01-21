#ifndef MUTEX_HEADER
#define MUTEX_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Events.h"

#define MAX_MUTEX_LENGTH 200

BOOL lock_mutex(char* mutex_name);

BOOL unlock_mutex(char* mutex_name);

void close_mutex(char* mutex_name);

HANDLE open_mutex(char* mutex_name);

#endif