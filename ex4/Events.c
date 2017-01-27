#include "Events.h"

//this function initialize the events//
HANDLE InitEvent(char *event_name)
{
	DWORD last_error = 0;
	HANDLE evnt = NULL;
	last_error = GetLastError();
	evnt = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)event_name);
	if(evnt == NULL)
	{
		printf("ERROR in GetExitCodeProcess function\n");
		write_log("!!! ERROR in GetExitCodeProcess function. Error code: 0x%x !!!\n", GetLastError()); 
		return FALSE;
	}
	SetLastError(last_error);

	return evnt;
}