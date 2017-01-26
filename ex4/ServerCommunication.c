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
		memset(data->message, '\0', MAX_COMMAND_LENGTH);
		if(receive_from_socket(data->socket, data->message) == FALSE)
		{
			write_log_and_print("Error while receiving message from user: %s, ErrorCode: 0x%x\n", data->username, GetLastError());
			return;
		}

		HandleIncomingMessage(data);
	}
}

void HandleIncomingMessage(communication_data* data)
{
	lock_mutex(HANDLE_INCOMING_MESSAGE_MUTEX);

	printf("Received message from user: %s, message: %s\n", data->username, data->message);
	write_log("Received message from user: %s, message: %s\n", data->username, data->message);
	//todo: bar You may use write_log_and_print function
	if(strstr(data->message, "message"))
	{
		HandleSendMessage(data);
	}
	else if(strstr(data->message, "broadcast"))
	{
		HandleBroadcastMessage(data);
	}
	else if(strstr(data->message, "players") == data->message)
	{
		//players
		//todo: notify game managment to handle this..
	}
	else if(strstr(data->message, "Player"))
	{
		//Player <game piece> (<username>) drew a <toss result>
		//todo: player did a move..
		// notify game management to handle this..
	}
	else
	{
		//todo: unknown message.. ignore?
	}

	unlock_mutex(HANDLE_INCOMING_MESSAGE_MUTEX);
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