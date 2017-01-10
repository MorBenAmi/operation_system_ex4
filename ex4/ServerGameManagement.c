#include "ServerGameManagement.h"

void start_server(int port)
{
	char users[NumOfPlayers][MaxUserNameLength];
	SOCKET user_sockets[NumOfPlayers];
	char* symbols = "@#%*";
	//todo: should be in a different thread?
	if(wait_for_players(port, user_sockets, users, symbols) == FALSE)
		return;
	write_to_log_order_of_players(user_sockets, users, symbols); 

}

void write_to_log_order_of_players(SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	//todo: implement...
}

BOOL wait_for_players(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	SOCKET listen_sock;
	int i = 0;
	int j = 0;
	char server_welcome_message[MaxServerWelcomeMessageLength];
	char player_joined_message[MaxPlayerJoinMessageLength];
	char player_symbol[2];
	
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
			close_connections(user_sockets);
			return FALSE;
		}

		lock_mutex(BROADCAST_MUTEX);

		if(receive_from_socket(user_sockets[i], users[i]) == FALSE)
		{
			printf("Failed to get username from user, Error_code: 0x%x",GetLastError());
			write_log("Failed to get username from user, Error_code: 0x%x",GetLastError());
			close_socket(user_sockets[i]);
			user_sockets[i] = INVALID_SOCKET;
			//todo: return False or should we continue receive new users instead?
			// if so.. don't forget to release the mutex!
		}
		else
		{
			memset(player_symbol, '\0', 2);
			player_symbol[0] = symbols[i];
			memset(server_welcome_message, '\0', MaxServerWelcomeMessageLength);
			strcat(server_welcome_message, users[i]);
			strcat(server_welcome_message, " your game piece is ");
			strcat(server_welcome_message, player_symbol);
			strcat(server_welcome_message, "\n");
			if(write_to_socket(user_sockets[i], server_welcome_message) == FALSE)
			{
				printf("Failed to send game piece to user, Error_code: 0x%x",GetLastError());
				write_log("Failed to send game piece to user, Error_code: 0x%x",GetLastError());
				close_socket(user_sockets[i]);
				user_sockets[i] = INVALID_SOCKET;
				//todo: return False or should we continue receive new users instead?
				// if so don't forget to release mutex!!
			}
			else
			{
				write_log(server_welcome_message);
				memset(player_joined_message, '\0', MaxPlayerJoinMessageLength);
				strcat(player_joined_message, "New player joined the game: ");
				strcat(player_joined_message, users[i]);
				strcat(player_joined_message, " ");
				strcat(player_joined_message, player_symbol);
				strcat(player_joined_message, "\n");
				for(j = 0; j < i; j++)
				{
					if(write_to_socket(user_sockets[i], player_joined_message) == FALSE)
					{
						//todo: what to do when this sending failed?!
					}
				}
			}
		}
		unlock_mutex(BROADCAST_MUTEX);
	}

	return TRUE;
}

void close_connections(SOCKET user_sockets[NumOfPlayers])
{
	int i =0;
	for(i = 0; i<NumOfPlayers; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			write_to_socket(user_sockets[i], "Close Connection\n");
			close_socket(user_sockets[i]);
		}
	}
}
