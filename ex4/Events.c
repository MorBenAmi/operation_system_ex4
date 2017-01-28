#include "Events.h"

//this function initialize the events//
HANDLE InitEvent(char *event_name)
{
	DWORD last_error = 0;
	HANDLE evnt = NULL;
	char unique_event_name[MAX_EVENT_LENGTH];

	memset(unique_event_name, '\0', MAX_EVENT_LENGTH);
	sprintf(unique_event_name, "%d_%s", GetCurrentProcessId(), event_name);

	last_error = GetLastError();
	evnt = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)unique_event_name);
	if(evnt == NULL)
	{
		write_log_and_print("!!! ERROR in CreateEvent function. Error code: 0x%x !!!\n", GetLastError()); 
		return FALSE;
	}
	SetLastError(last_error);

	return evnt;
}