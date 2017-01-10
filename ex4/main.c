/*Limor Mendelzburg 308081389
	Mor Ben Ami 203607536
	Exercise 4*/
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <stdio.h>
#include "Engine.h"
#include "ServerGameManagement.h"
#include "Log.h"
#include "SocketWrapper.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[])
{
	char* mode = NULL;
	char* log_path = NULL;
	int port = 0;
	char* username = NULL;

	mode = argv[1];
	port = atoi(argv[2]);
	log_path = argv[3];

	open_log(log_path); //todo: maybe need to change how log works because its uses Global variables
	//We will need to send it to the client/server

	if(init_WSA() == FALSE)
	{
		printf("Failed to init WSA, Error_Code: 0x%x", GetLastError());
	}
	if(strcmp(mode,"server") == 0)
	{
		start_server(port);
	}
	else if(strcmp(mode,"client") == 0)
	{
		username = argv[4];
		runClient(port, username);
	} 

	clean_WSA();
	close_log();
	exit(GetLastError());
}//main1