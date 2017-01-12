#include "Events.h"

//this function initialize the events//
BOOL InitEvent(HANDLE *evnt, char *event_name)
{
	*evnt = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)event_name);
	if(evnt == NULL)
	{
		printf("ERROR in GetExitCodeProcess function\n");
		write_log("!!! ERROR in GetExitCodeProcess function. Error code: 0x%x !!!\n", GetLastError()); 
		return FALSE;
	}

	return TRUE;
}