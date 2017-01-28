#include "ServerGameManagement.h"

void start_server(int port)
{
	char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH];
	SOCKET user_sockets[MAX_NUM_OF_PLAYERS] = {INVALID_SOCKET};
	char symbols[MAX_NUM_OF_PLAYERS] = {'@','#','%','*'};
	communication_data players_communication_data[MAX_NUM_OF_PLAYERS];
	HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS] = {NULL};
	int num_of_threads = 0;
	HANDLE all_threads_must_end_event;
	
	all_threads_must_end_event = InitEvent("AllThreadsMustEnd");
	if(all_threads_must_end_event == NULL)
	{
		write_log("Failed to create event!. Error_code: 0x%x\n", GetLastError());
		return;
	}

	if(WaitForPlayers(port, user_sockets, users, symbols, players_communication_data, players_communication_thread, all_threads_must_end_event) == FALSE)
	{
		CloseConnections(user_sockets);
		return;
	}

	WriteToLogOrderOfPlayers(user_sockets, users, symbols); 
	if(BroadcastPlayers(user_sockets, users, symbols) == FALSE)
	{
		write_log("Failed to send players list to all players, Error_code: 0x%x\n", GetLastError());
		CloseConnections(user_sockets);
		return;
	}

	if(PlayGame(users, user_sockets, symbols, all_threads_must_end_event) == FALSE)
	{
		write_log("Failed to play game, Error_code: 0x%x\n", GetLastError());
		CloseConnections(user_sockets);
		return;
	}

	while(players_communication_thread[num_of_threads] != NULL)
		num_of_threads++;
	WaitForMultipleObjects(num_of_threads, players_communication_thread, TRUE, INFINITE);

	CloseConnections(user_sockets);
}

BOOL WaitForPlayers(int port, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], 
	char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS], 
	communication_data players_communications[MAX_NUM_OF_PLAYERS], HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS],
	HANDLE all_threads_must_end_event)
{
	SOCKET listen_sock;
	int i = 0;
	int connected_users_count = 0;
	time_t start_time;
	time_t stop_time;
	char player_symbol[2];
	
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		user_sockets[i] = INVALID_SOCKET;
		memset(users[i],'\0', MAX_USER_NAME_LENGTH);
	}

	if(sock_listen(port, MAX_NUM_OF_PLAYERS, &listen_sock) == FALSE)
	{
		printf("Failed on listening port:%d, Error_code: 0x%x\n",port, GetLastError());
		write_log("Failed on listening port:%d, Error_code: 0x%x\n",port, GetLastError());
		return FALSE;
	}

	time(&start_time);
	while(connected_users_count < MAX_NUM_OF_PLAYERS)
	{
		//wait for connection to occure
		while(user_sockets[connected_users_count] == INVALID_SOCKET)
		{
			time(&stop_time);
			if(difftime(stop_time,start_time) > MAX_TIME_TO_WAIT_FOR_PLAYERS)
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

		if(ReceiveUsername(user_sockets[connected_users_count], users[connected_users_count]) == FALSE)
		{
			close_socket(listen_sock);
			unlock_mutex(BROADCAST_MUTEX);
			return FALSE;
		}

		if(IsUsernameExists(users[connected_users_count], users, connected_users_count) == TRUE)
		{
			write_to_socket(user_sockets[connected_users_count], "Cannot accept connection. Username already exists\n");
			close_socket(user_sockets[connected_users_count]);			
			user_sockets[connected_users_count] = INVALID_SOCKET;
			memset(users[connected_users_count],'\0', MAX_USER_NAME_LENGTH);
			unlock_mutex(BROADCAST_MUTEX);
			continue;
		}

		if(SendWelcomeMessage(user_sockets[connected_users_count], users[connected_users_count], symbols[connected_users_count]) == FALSE)
		{
			close_socket(listen_sock);
			unlock_mutex(BROADCAST_MUTEX);
			return FALSE;
		}

		if(BroadcastNewPlayerJoined(user_sockets, connected_users_count, users[connected_users_count], symbols[connected_users_count]) == FALSE)
		{
			close_socket(listen_sock);
			unlock_mutex(BROADCAST_MUTEX);
			return FALSE;
		}
		
		players_communications[connected_users_count].socket = user_sockets[connected_users_count];
		strcpy(players_communications[connected_users_count].username, users[connected_users_count]);
		players_communications[connected_users_count].symbol = symbols[connected_users_count];
		memset(players_communications[connected_users_count].message, '\0', MAX_COMMAND_LENGTH);
		players_communications[connected_users_count].all_users_sockets = user_sockets;
		players_communications[connected_users_count].all_users = users;
		players_communications[connected_users_count].all_symbols = symbols;
		players_communications[connected_users_count].all_threads_must_end_event = all_threads_must_end_event;

		players_communication_thread[connected_users_count] = CreateThread(NULL, 0, ServerCommunicationThreadStart, &(players_communications[connected_users_count]), 0, NULL);
		if(players_communication_thread[connected_users_count] == NULL)
		{
			printf("Failed to create ServerCommunicationThread for user: %s, ErrorCode: 0x%x\n", users[connected_users_count], GetLastError());
			write_log("Failed to create ServerCommunicationThread for user: %s, ErrorCode: 0x%x\n", users[connected_users_count], GetLastError());
			close_socket(listen_sock);
			unlock_mutex(BROADCAST_MUTEX);
			return FALSE;
		}

		connected_users_count++;
		unlock_mutex(BROADCAST_MUTEX);
	}

	close_socket(listen_sock);
	return TRUE;
}

BOOL PlayGame(char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char symbols[MAX_NUM_OF_PLAYERS], HANDLE all_threads_must_end_event)
{
	int current_player = 0;
	int i = 0;
	int num_of_players = 0;
	char player_turn_message[MAX_PLAYER_TURN_MESSAGE_LENGTH];
	HANDLE wait_handles[3];
	DWORD wait_result = 0;

	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
			num_of_players++;
	}

	wait_handles[0] = InitEvent("TurnFinished");
	if(wait_handles[0] == NULL)
	{
		write_log("Failed creating TurnFinished Event, Error_code: 0x%x\n", GetLastError());
		return FALSE;
	}
	wait_handles[1] = InitEvent("PlayerWon");
	if(wait_handles[1] == NULL)
	{
		write_log("Failed creating PlayerWon Event, Error_code: 0x%x\n", GetLastError());
		return FALSE;
	}
	wait_handles[2] = all_threads_must_end_event;

	while(1)
	{
		if(ShouldFinishExecution(all_threads_must_end_event) == TRUE)
			return FALSE;

		lock_mutex(BROADCAST_MUTEX);

		write_log("Your turn to play.\n");
		if(write_to_socket(user_sockets[current_player], "Your turn to play.\n") == FALSE)
		{
			write_log("Failed to write to socket, Error_code: 0x%x\n", GetLastError());
			unlock_mutex(BROADCAST_MUTEX);
			return FALSE;
		}
		
		memset(player_turn_message, '\0', MAX_PLAYER_TURN_MESSAGE_LENGTH);
		sprintf(player_turn_message, "It is now %s's turn to play.\n", users[current_player]);
		write_log(player_turn_message);
		for(i = 0; i < num_of_players; i++)
		{
			if(i != current_player)
			{
				if(write_to_socket(user_sockets[i], player_turn_message) == FALSE)
				{
					unlock_mutex(BROADCAST_MUTEX);
					return FALSE;
				}
			}
		}

		unlock_mutex(BROADCAST_MUTEX);

		wait_result = WaitForMultipleObjects(3, wait_handles, FALSE, INFINITE);
		switch(wait_result)
		{
			case WAIT_OBJECT_0: // Turn Finished
				ResetEvent(wait_handles[0]);
				break;
			case WAIT_OBJECT_0 + 1: // Player Won
				return TRUE;
				break;
			case WAIT_OBJECT_0 + 2: // all threads must end
				return FALSE;
				break;
			default:
				write_log("Unexpected wait result, result: %d, Error_code: 0x%x\n", wait_result, GetLastError());
				return FALSE;
				break;
		}

		current_player++;
		current_player = current_player % num_of_players;
	}
}

void WriteToLogOrderOfPlayers(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS])
{
	int i = 0;
	write_log("The order of players' in the game is ");
	for(i = 0; i<MAX_NUM_OF_PLAYERS; i++)
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

BOOL BroadcastPlayers(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS])
{
	int i = 0;

	lock_mutex(BROADCAST_MUTEX);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			if(SendPlayersToUser(user_sockets[i], user_sockets, users, symbols) == FALSE)
			{
				unlock_mutex(BROADCAST_MUTEX);
				return FALSE;
			}
		}
	}
	unlock_mutex(BROADCAST_MUTEX);
	write_log("Players' game pieces' selection broadcasted to all users.\n");
	return TRUE;
}

BOOL SendPlayersToUser(SOCKET user_sock, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS])
{
	char players_message[MAX_PLAYERS_LIST_MESSAGE_LENGTH];
	int i = 0;
	int j = 0;

	memset(players_message, '\0', MAX_PLAYERS_LIST_MESSAGE_LENGTH);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
		{
			if(i > 0)
				j += sprintf(players_message + j, ", ");
			j += sprintf(players_message + j, "%s-%c", users[i], symbols[i]);
		}
	}
	strcat(players_message, ".\n");
	return write_to_socket(user_sock, players_message);
}

void CloseConnections(SOCKET user_sockets[MAX_NUM_OF_PLAYERS])
{
	int i =0;

	lock_mutex(BROADCAST_MUTEX);
	for(i = 0; i<MAX_NUM_OF_PLAYERS; i++)
	{
		if(user_sockets[i] != INVALID_SOCKET)
			close_socket(user_sockets[i]);
	}
	unlock_mutex(BROADCAST_MUTEX);
}

BOOL ReceiveUsername(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH])
{
	char username_message[MAX_USER_NAME_MESSAGE_LENGTH];
	char* username_prefix;
	char* username_suffix;
	
	memset(username_message, '\0', MAX_USER_NAME_MESSAGE_LENGTH);
	if(receive_from_socket(user_sock, username_message) == FALSE)
	{
		printf("Failed to receive username message, ErrorCode: 0x%x\n", GetLastError());
		write_log("Failed to receive username message, ErrorCode: 0x%x\n", GetLastError());
		return FALSE;	
	}

	username_prefix = strtok(username_message, "=");
	username_suffix = strtok(NULL, "\n");
	if(strcmp(username_prefix, "username") != 0)
	{
		printf("Unexpected username message received! received message: %s", username_message);
		write_log("Unexpected username message received! received message: %s", username_message);
		return FALSE;
	}
	strcpy(username, username_suffix);
	return TRUE;
}

BOOL SendWelcomeMessage(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH], char user_symbol)
{
	char server_welcome_message[MAX_WELCOME_MESSAGE_LENGTH];
	
	memset(server_welcome_message, '\0', MAX_WELCOME_MESSAGE_LENGTH);
	sprintf(server_welcome_message, "%s your game piece is %c\n", username, user_symbol);
	write_log(server_welcome_message);
	if(write_to_socket(user_sock, server_welcome_message) == FALSE)
	{
		printf("Failed to send game piece to user, Error_code: 0x%x\n",GetLastError());
		write_log("Failed to send game piece to user, Error_code: 0x%x\n",GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL BroadcastNewPlayerJoined(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], int index_of_new_player, char new_player_username[MAX_USER_NAME_LENGTH], char new_player_symbol)
{
	char player_joined_message[MAX_PLAYER_JOINED_MESSAGE_LENGTH];
	char new_player_symbol_str[2];
	int i = 0;

	memset(player_joined_message, '\0', MAX_PLAYER_JOINED_MESSAGE_LENGTH);
	sprintf(player_joined_message, "New player joined the game: %s %c\n", new_player_username, new_player_symbol);
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

BOOL IsUsernameExists(char username[MAX_USER_NAME_LENGTH], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], int user_index)
{
	int i = 0;

	for(i = 0; i < user_index; i++)
	{
		if(strcmp(username, users[i]) == 0)
			return TRUE;
	}
	return FALSE;
}

BOOL ShouldFinishExecution(HANDLE all_threads_must_end_event)
{
	DWORD allThreadsMustEndSignal = WaitForSingleObject(all_threads_must_end_event,0);
	switch(allThreadsMustEndSignal)
	{
		case WAIT_OBJECT_0:
			return TRUE;
			break;
		case WAIT_TIMEOUT:
			return FALSE;
			break;
		default:
			return TRUE;
			break;
	}
}