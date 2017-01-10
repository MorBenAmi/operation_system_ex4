#include "ServerGameManagement.h"

void start_server(int port)
{
	char users[NumOfPlayers][MaxUserNameLength];
	SOCKET user_sockets[NumOfPlayers];
	char* symbols = "@#%*";
	//todo: should be in a different thread?
	if(WaitForPlayers(port, user_sockets, users, symbols) == FALSE)
		return;

}

BOOL WaitForPlayers(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	SOCKET listen_sock;
	int i = 0;
	int j = 0;
	char server_welcome_message[MaxServerWelcomeMessageLength];
	
	for(i = 0; i < NumOfPlayers; i++)
	{
		user_sockets[i] = INVALID_SOCKET;
		memset(users[i],'\0', MaxUserNameLength);
	}

	if(sock_listen(port, &listen_sock) == FALSE)
	{
		printf("Failed on listening port:%d, Error_code: 0x%x",port, GetLastError());
		write_log("Failed on listening port:%d, Error_code: 0x%x",port, GetLastError());
		return FALSE;
	}
	for(i=0;i<NumOfPlayers;i++)
	{
		if(accept_connection(listen_sock, &user_sockets[i]) == FALSE)
		{
			CloseConnections(user_sockets);
			return FALSE;
		}
		if(receive_from_socket(user_sockets[i], users[i]) == FALSE)
		{
			printf("Failed to get username from user, Error_code: 0x%x",GetLastError());
			write_log("Failed to get username from user, Error_code: 0x%x",GetLastError());
			close_socket(user_sockets[i]);
			user_sockets[i] = INVALID_SOCKET;
			//todo: return False or should we continue receive new users instead?
		}
		else
		{
			memset(server_welcome_message, '\0', MaxServerWelcomeMessageLength);
			strcat(server_welcome_message, users[i]);
			strcat(server_welcome_message, " your game piece is ");
			strcat(server_welcome_message, &(symbols[i]));
			if(write_to_socket(user_sockets[i], server_welcome_message) == FALSE)
			{
				printf("Failed to send game piece to user, Error_code: 0x%x",GetLastError());
				write_log("Failed to send game piece to user, Error_code: 0x%x",GetLastError());
				close_socket(user_sockets[i]);
				user_sockets[i] = INVALID_SOCKET;
				//todo: return False or should we continue receive new users instead?
			}
		}
	}

	return TRUE;
}

void CloseConnections(SOCKET user_sockets[NumOfPlayers])
{
	int i =0;
	for(i = 0; i<NumOfPlayers; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			write_to_socket(user_sockets[i], "Close Connection");
			close_socket(user_sockets[i]);
		}
	}
}
