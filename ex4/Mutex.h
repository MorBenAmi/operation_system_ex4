#ifndef MUTEX_HEADER
#define MUTEX_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Events.h"

BOOL lock_mutex(char* mutex_name);
BOOL lock_mutex_num(unsigned long int mutex_num);

BOOL unlock_mutex(char* mutex_name);
BOOL unlock_mutex_num(unsigned long int mutex_num);

void close_mutex(char* mutex_name);
void close_mutex_num(unsigned long int mutex_num);

#endif