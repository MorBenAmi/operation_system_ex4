#include "ServerCommunication.h"

DWORD WINAPI ServerCommunicationThreadStart(LPVOID param)
{
	communication_data *data = (communication_data*)param;
	SetLastError(0);
	StartServerCommunication(data);
	return GetLastError();
}

void StartServerCommunication(communication_data* data)
{
	while(1)
	{
		if(ShouldFinishThread(data->all_threads_must_end_event) == TRUE)
			return;

		memset(data->message, '\0', MAX_COMMAND_LENGTH);
		if(receive_from_socket(data->socket, data->message) == FALSE)
		{
			write_log_and_print("Error while receiving message from user: %s, ErrorCode: 0x%x\n", data->username, GetLastError());
			return;
		}

		if(HandleIncomingMessage(data) == FALSE)
		{
			SetEvent(data->all_threads_must_end_event);
			return;
		}
	}
}

BOOL HandleIncomingMessage(communication_data* data)
{
	BOOL handle_result = TRUE;

	lock_mutex(HANDLE_INCOMING_MESSAGE_MUTEX);

	printf("Received message from user: %s, message: %s\n", data->username, data->message);
	write_log("Received message from user: %s, message: %s\n", data->username, data->message);

	if(strstr(data->message, "message") == data->message)
	{
		lock_mutex(BROADCAST_MUTEX);
		handle_result = HandleSendMessage(data);
		unlock_mutex(BROADCAST_MUTEX);
	}
	else if(strstr(data->message, "broadcast") == data->message)
	{
		lock_mutex(BROADCAST_MUTEX);
		handle_result = HandleBroadcastMessage(data);
		unlock_mutex(BROADCAST_MUTEX);
	}
	else if(strstr(data->message, "players") == data->message)
	{
		lock_mutex(BROADCAST_MUTEX);
		handle_result = HandlePlayersMessage(data);
		unlock_mutex(BROADCAST_MUTEX);
	}
	else if(strstr(data->message, "Player") == data->message)
	{
		if(strstr(data->message, "drew") != NULL)
		{
			lock_mutex(BROADCAST_MUTEX);
			handle_result = HandlePlayerTurnMessage(data);
			unlock_mutex(BROADCAST_MUTEX);
		}
		else if(strstr(data->message, "won") != NULL)
		{
			lock_mutex(BROADCAST_MUTEX);
			handle_result = HandlePlayeWonMessage(data);
			unlock_mutex(BROADCAST_MUTEX);
		}
		else
		{
			//todo: unknown message.. ignore?
		}
	}
	else
	{
		//todo: unknown message.. ignore?
	}

	unlock_mutex(HANDLE_INCOMING_MESSAGE_MUTEX);

	return handle_result;
}

BOOL HandleSendMessage(communication_data* data)
{
	//message <user> <message>
	char *command_name = NULL;
	char *username = NULL;
	char *message = NULL;
	int i = 0;
	char private_message[MAX_PRIVATE_MESSAGE_LENGTH];
	char user_not_exist_message[MAX_USER_NOT_EXIST_MESSAGE_LENGTH];

	command_name = strtok(data->message, " ");
	username = strtok(NULL, " ");
	message = strtok(NULL, "\n");

	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(strcmp(username, data->all_users[i]) == 0)
		{
			memset(private_message, '\0', MAX_PRIVATE_MESSAGE_LENGTH);
			sprintf(private_message, "Private message from %s: %s\n", data->username, message);
			write_log("Private message sent from %s to %s: %s\n", data->username, username, message);
			if(write_to_socket(data->all_users_sockets[i], private_message) == FALSE)
				return FALSE;

			return TRUE;
		}
	}

	//user not exist..
	memset(user_not_exist_message, '\0', MAX_USER_NOT_EXIST_MESSAGE_LENGTH);
	sprintf(user_not_exist_message, "User %s  doesn't exist in the game.\n", username);
	write_log(user_not_exist_message);
	if(write_to_socket(data->socket, user_not_exist_message) == FALSE)
		return FALSE;

	return TRUE;
}

BOOL HandleBroadcastMessage(communication_data* data)
{
	//broadcast <message>
	char *command_name = NULL;
	char *message = NULL;
	int i = 0;
	char broadcast_message[MAX_BROADCAST_MESSAGE_LENGTH];
	char user_not_exist_message[MAX_USER_NOT_EXIST_MESSAGE_LENGTH];

	command_name = strtok(data->message, " ");
	message = strtok(NULL, "\n");

	memset(broadcast_message, '\0', MAX_PRIVATE_MESSAGE_LENGTH);
	sprintf(broadcast_message, "Broadcast from %s: %s\n", data->username, message);
	write_log("Broadcast message from user %s: %s\n", data->username, message);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(data->all_users_sockets[i] != INVALID_SOCKET && data->all_users_sockets[i] != data->socket)
		{
			if(write_to_socket(data->all_users_sockets[i], broadcast_message) == FALSE)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL HandlePlayerTurnMessage(communication_data* data)
{
	//Player <game piece> (<username>) drew a <toss result>
	int i = 0;
	HANDLE turn_finished_event = NULL;

	write_log(data->message);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(data->all_users_sockets[i] != INVALID_SOCKET && data->all_users_sockets[i] != data->socket)
		{
			if(write_to_socket(data->all_users_sockets[i], data->message) == FALSE)
			{
				write_log("Failed to send player draw result, Error_code: 0x%x\n", GetLastError());
				return FALSE;
			}
		}
	}

	turn_finished_event = InitEvent("TurnFinished");
	if(turn_finished_event == NULL)
	{
		write_log("Failed to create TurnFinished Event, Error_code: 0x%x\n", GetLastError());
		return FALSE;
	}

	SetEvent(turn_finished_event);
	return TRUE;
}

BOOL HandlePlayeWonMessage(communication_data* data)
{
	int i = 0;
	HANDLE won_event = NULL;

	write_log(data->message);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(data->all_users_sockets[i] != INVALID_SOCKET && data->all_users_sockets[i] != data->socket)
		{
			if(write_to_socket(data->all_users_sockets[i], data->message) == FALSE)
			{
				write_log("Failed to send player won message, Error_code: 0x%x\n", GetLastError());
				return FALSE;
			}
		}
	}
	won_event = InitEvent("PlayerWon");
	if(won_event == NULL)
	{
		write_log("Failed to create PlayerWon Event");
		return FALSE;
	}
	SetEvent(won_event);
	return TRUE;
}

BOOL HandlePlayersMessage(communication_data* data)
{
	char players_message[MAX_PLAYERS_LIST_MESSAGE_LENGTH];
	int i = 0;
	int j = 0;
	BOOL send_result;

	memset(players_message, '\0', MAX_PLAYERS_LIST_MESSAGE_LENGTH);
	for(i = 0; i < MAX_NUM_OF_PLAYERS; i++)
	{
		if(data->all_users_sockets[i] != INVALID_SOCKET)
		{
			if(i > 0)
				j += sprintf(players_message + j, ",");
			j += sprintf(players_message + j, "%s-%c", data->all_users[i], data->all_symbols[i]);
		}
	}
	strcat(players_message, ".\n");
	send_result = write_to_socket(data->socket, players_message);

	return send_result;
}

BOOL ShouldFinishThread(HANDLE all_threads_must_end_event)
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