#include "Events.h"

HANDLE AllThreadsMustEnd;
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//the file containe all the shared events in the program// 
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//this function initialize the events//
BOOL InitEvents()
{
	AllThreadsMustEnd = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"AllThreadsMustEnd");
	if(AllThreadsMustEnd == NULL)
	{
		printf("ERROR in GetExitCodeProcess function\n");
		write_log("!!! ERROR in GetExitCodeProcess function. Error code: 0x%x !!!\n", GetLastError()); 
		return FALSE;
	}

	return TRUE;
}