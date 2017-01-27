#ifndef EVENTS_HEADER
#define EVENTS_HEADER
#include <Windows.h>
#include "Log.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_EVENT_LENGTH 100

HANDLE InitEvent(char *event_name);

#endif
