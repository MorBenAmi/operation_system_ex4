#include "ServerGameManagement.h"

void start_server(int port)
{
	char users[NumOfPlayers][MaxUserNameLength];
	SOCKET user_sockets[NumOfPlayers];
	char symbols[NumOfPlayers];
	symbols[0] = '@';
	symbols[1] = '#';
	symbols[2] = '%';
	symbols[3] = '*';

	if(wait_for_players(port, user_sockets, users, symbols) == FALSE)
	{
		close_connections(user_sockets);
		return;
	}

	write_to_log_order_of_players(user_sockets, users, symbols); 
	if(broadcast_players(user_sockets, users, symbols) == FALSE)
	{
		write_log("Failed to send players list to all players, Error_code: 0x%x\n", GetLastError());
		close_connections(user_sockets);
		return;
	}

	close_connections(user_sockets);
}

void write_to_log_order_of_players(SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	int i = 0;
	write_log("The order of players' in the game is ");
	for(i = 0; i<NumOfPlayers; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			if(i > 0)
				write_log(",");
			write_log("%s",users[i]);
		}
	}
	write_log(".\n");
}

BOOL broadcast_players(SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	int i = 0;

	lock_mutex(BROADCAST_MUTEX);
	for(i = 0; i < NumOfPlayers; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			if(send_players_to_user(user_sockets[i], user_sockets, users, symbols) == FALSE)
			{
				unlock_mutex(BROADCAST_MUTEX);
				return FALSE;
			}
		}
	}
	unlock_mutex(BROADCAST_MUTEX);
	write_log("Players' game pieces' selection broadcasted to all users.");
	return TRUE;
}

BOOL send_players_to_user(SOCKET user_sock, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	char players_message[MaxPlayersMessageLength];
	char user_symbol[2];
	int i = 0;

	memset(user_symbol, '\0', 2);
	memset(players_message, '\0', MaxPlayersMessageLength);
	for(i = 0; i < NumOfPlayers; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			user_symbol[0] = symbols[i];
			if(i > 0)
				strcat(players_message, ",");
			strcat(players_message, users[i]);
			strcat(players_message, "-");
			strcat(players_message, user_symbol);
		}
	}
	strcat(players_message, ".\n");
	return write_to_socket(user_sock, players_message);
}

BOOL wait_for_players(int port, SOCKET user_sockets[NumOfPlayers], char users[NumOfPlayers][MaxUserNameLength], char symbols[NumOfPlayers])
{
	SOCKET listen_sock;
	int i = 0;
	int connected_users_count = 0;
	time_t start_time;
	time_t stop_time;
	char server_welcome_message[MaxServerWelcomeMessageLength];
	char player_joined_message[MaxPlayerJoinMessageLength];
	char player_symbol[2];
	
	for(i = 0; i < NumOfPlayers; i++)
	{
		user_sockets[i] = INVALID_SOCKET;
		memset(users[i],'\0', MaxUserNameLength);
	}

	if(sock_listen(port, NumOfPlayers, &listen_sock) == FALSE)
	{
		printf("Failed on listening port:%d, Error_code: 0x%x\n",port, GetLastError());
		write_log("Failed on listening port:%d, Error_code: 0x%x\n",port, GetLastError());
		return FALSE;
	}

	time(&start_time);
	while(connected_users_count < NumOfPlayers)
	{
		//wait for connection to occure
		while(user_sockets[connected_users_count] == INVALID_SOCKET)
		{
			time(&stop_time);
			if(difftime(stop_time,start_time) > 60)
			{
				if(i == 0)
				{
					printf("No players connected, exiting...\n");
					write_log("No players connected, exiting...\n");
					close_socket(listen_sock);
					return FALSE;
				}
				else
				{
					close_socket(listen_sock);
					return TRUE;
				}
			}
			Sleep(100);
			if(accept_connection(listen_sock, &user_sockets[connected_users_count]) == FALSE)
			{
				close_socket(listen_sock);
				return FALSE;
			}
		}

		lock_mutex(BROADCAST_MUTEX);

		if(receive_username(user_sockets[connected_users_count], users[connected_users_count]) == FALSE)
		{
			unlock_mutex(BROADCAST_MUTEX);
			close_socket(listen_sock);
			return FALSE;
		}

		if(username_exists(users[connected_users_count], users, connected_users_count) == TRUE)
		{
			//todo: write to socke that the username is already exist..
			write_to_socket(user_sockets[connected_users_count], "Cannot accept connection. Username already exists\n");
			close_socket(user_sockets[connected_users_count]);
			user_sockets[connected_users_count] = INVALID_SOCKET;
			memset(users[connected_users_count],'\0', MaxUserNameLength);
			continue;
		}

		if(send_welcome_message(user_sockets[connected_users_count], users[connected_users_count], symbols[connected_users_count]) == FALSE)
		{
			unlock_mutex(BROADCAST_MUTEX);
			close_socket(listen_sock);
			return FALSE;
		}

		if(broadcast_new_player_joined(user_sockets, connected_users_count, users[connected_users_count], symbols[connected_users_count]) == FALSE)
		{
			unlock_mutex(BROADCAST_MUTEX);
			close_socket(listen_sock);
			return FALSE;
		}
		
		unlock_mutex(BROADCAST_MUTEX);

		connected_users_count++;
	}

	close_socket(listen_sock);
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

BOOL receive_username(SOCKET user_sock, char username[MaxUserNameLength])
{
	char username_message[MaxUserNameMessageLength];
	char* username_prefix;
	char* username_suffix;
	
	memset(username_message, '\0', MaxUserNameMessageLength);
	if(receive_from_socket(user_sock, username_message) == FALSE)
	{
		printf("Failed to receive username message, ErrorCode: 0x%x\n", GetLastError());
		write_log("Failed to receive username message, ErrorCode: 0x%x\n", GetLastError());
		return FALSE;	
	}

	username_prefix = strtok(username_message, "=");
	username_suffix = strtok(NULL, "=");
	if(strcmp(username_prefix, "username") != 0)
	{
		printf("Unexpected username message received! received message: %s", username_message);
		write_log("Unexpected username message received! received message: %s", username_message);
		return FALSE;
	}
	strcpy(username, username_suffix);
	return TRUE;
}

BOOL send_welcome_message(SOCKET user_sock, char username[MaxUserNameLength], char user_symbol)
{
	char server_welcome_message[MaxServerWelcomeMessageLength];
	char user_symbol_str[2];

	user_symbol_str[0] = user_symbol;
	user_symbol_str[1] = '\0';
	
	memset(server_welcome_message, '\0', MaxServerWelcomeMessageLength);
	strcat(server_welcome_message, username);
	strcat(server_welcome_message, " your game piece is ");
	strcat(server_welcome_message, user_symbol_str);
	strcat(server_welcome_message, "\n");

	write_log(server_welcome_message);
	if(write_to_socket(user_sock, server_welcome_message) == FALSE)
	{
		printf("Failed to send game piece to user, Error_code: 0x%x\n",GetLastError());
		write_log("Failed to send game piece to user, Error_code: 0x%x\n",GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL broadcast_new_player_joined(SOCKET user_sockets[NumOfPlayers], int index_of_new_player, char new_player_username[MaxUserNameLength], char new_player_symbol)
{
	char player_joined_message[MaxPlayerJoinMessageLength];
	char new_player_symbol_str[2];
	int i = 0;

	new_player_symbol_str[0] = new_player_symbol;
	new_player_symbol_str[1] = '\0';

	memset(player_joined_message, '\0', MaxPlayerJoinMessageLength);
	strcat(player_joined_message, "New player joined the game: ");
	strcat(player_joined_message, new_player_username);
	strcat(player_joined_message, " ");
	strcat(player_joined_message, new_player_symbol_str);
	strcat(player_joined_message, "\n");

	write_log(player_joined_message);
	for(i = 0; i < index_of_new_player; i++)
	{
		if(write_to_socket(user_sockets[i], player_joined_message) == FALSE)
		{
			printf("Failed to send player joined message! ErrorCode: 0x%x\n", GetLastError());
			write_log("Failed to send player joined message! ErrorCode: 0x%x\n", GetLastError());
			return FALSE;
		}
	}

	return TRUE;
}

BOOL username_exists(char username[MaxUserNameLength], char users[NumOfPlayers][MaxUserNameLength], int user_index)
{
	int i = 0;

	for(i = 0; i < user_index; i++)
	{
		if(strcmp(username, users[i]) == 0)
			return TRUE;
	}
	return FALSE;
}