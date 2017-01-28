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
#define SERVER_MODE "server"
#define CLIENT_MODE "client"


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[])
{
	char* mode = NULL;
	char* log_path = NULL;
	int port = 0;
	char* username = NULL;

	mode = argv[1];
	log_path = argv[2];
	port = atoi(argv[3]);

	open_log(log_path); 

	if(init_WSA() == FALSE)
	{
		printf("Failed to init WSA, Error_Code: 0x%x\n", GetLastError());
	}

	if(strcmp(mode,SERVER_MODE) == 0)
	{
		start_server(port);
	}
	else if(strcmp(mode, CLIENT_MODE) == 0)
	{
		username = argv[4];
		RunClient(port, username);
	} 

	clean_WSA();
	close_log();
	exit(GetLastError());
}//main1