#include "Engine.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void RunClient(int port, char *username)
{
	HANDLE mutexes[2]={NULL};
	HANDLE threads[2]={NULL};
	DWORD lock_result;
	game_board board;
	data_ui ui;
	data_communication communication;
	communication.port = port;
	communication.username = username;
	
	if (!ConnectToServer(&communication)) 
	{
		ExitGame(&communication, &ui, threads);
	}
	
	threads[0] = RunUiThread(&ui);
	if (threads[0] == NULL)
	{
		ExitGame(&communication, &ui, threads);
	}
	threads[1] = RunClientCommunicationThread(&communication);
	if (threads[1] == NULL)
	{
		ExitGame(&communication, &ui, threads);
	}
	mutexes[0] = ui.UserEnteredTextSemaphore;
	mutexes[1] = communication.IncomingMessageFromServerSemaphore;
	
	//init random
	srand(time(NULL));

	BuildBoard(&board);

	while (1) 
	{
		lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:				
				if (ReceivedUserMessage(&communication, &ui, &board) == TRUE)
					ExitGame(&communication, &ui, threads);
				break;
			case WAIT_OBJECT_0 + 1:
				if (HandleServerMessage(&communication, &ui, &board) == TRUE)
					ExitGame(&communication, &ui, threads);
				break;
		}
	}
	ExitGame(&communication, &ui, threads);
}

BOOL ConnectToServer(data_communication *communication) 
{
	char username_message[MAX_SIZE_OF_USERNAME_MESSAGE];
	if (connect_socket(communication->port, &communication->socket) == TRUE) 
	{
		write_log_and_print("Connected to server on port %d\n", communication->port);
		
		memset(username_message, '\0', MAX_SIZE_OF_USERNAME_MESSAGE);
		strcat(username_message, "username=");
		strcat(username_message, communication->username);
		strcat(username_message, "\n");

		if (SendMessageToServer(communication->socket, username_message) == FALSE ) 
		{
			printf("Socket error while trying to write data to socket\n");
			//todo error
			return FALSE;
		}
		return TRUE;
	}
	else 
	{
		write_log_and_print("Failed connecting to server on port %d\n", communication->port);		
		return FALSE;
	}
}

HANDLE RunClientCommunicationThread(data_communication *communication) 
{
	HANDLE clientCommunicationHandle = NULL;

	communication->IncomingMessageFromServerSemaphore = 
		CreateSemaphoreSimple("IncomingMessageFromServerSemaphore");
	if (communication->IncomingMessageFromServerSemaphore == NULL)
	{
		write_log_and_print("Failed to create semaphore - Error code: 0x%x\n", GetLastError());
		return NULL;
	}
	communication->EngineDoneWithServerMessageSemaphore = 
		CreateSemaphoreSimple("EngineDoneWithServerMessageSemaphore");
	if (communication->EngineDoneWithServerMessageSemaphore == NULL)
	{
		write_log_and_print("Failed to create semaphore - Error code: 0x%x\n", GetLastError());
		return NULL;
	}

	clientCommunicationHandle = CreateThread(NULL, 0, RunClientCommunication, communication, 0, NULL);
	if(clientCommunicationHandle == NULL)
	{
		write_log_and_print("Failed to create thread - Error code: 0x%x\n", GetLastError());
	}
	return clientCommunicationHandle;
}

HANDLE RunUiThread(data_ui *ui) 
{
	HANDLE uiHandle = NULL;

	ui->UserEnteredTextSemaphore = 
		CreateSemaphoreSimple("UserEnteredTextSemaphore");
	if (ui->UserEnteredTextSemaphore == NULL)
	{
		write_log_and_print("Failed to create semaphore - Error code: 0x%x\n", GetLastError());
		return NULL;
	}
	ui->EngineDoneWithUserMessageSemaphore = 
		CreateSemaphoreSimple("EngineDoneWithUserMessageSemaphore");
	
	if (ui->EngineDoneWithUserMessageSemaphore == NULL)
	{
		write_log_and_print("Failed to create semaphore - Error code: 0x%x\n", GetLastError());
		return NULL;
	}

	uiHandle = CreateThread(NULL, 0, RunUiManager, ui, 0, NULL);
	if(uiHandle == NULL)
	{
		write_log_and_print("Failed to create thread - Error code: 0x%x\n", GetLastError());
	}
	return uiHandle;
}

BOOL ReceivedUserMessage(data_communication *communication, data_ui *ui, game_board *board)
{
	DWORD return_value;
	char *token=NULL;
	int num_of_args;
	char command_copy[MAX_COMMAND_LENGTH];
	strcpy(command_copy, ui->command);
	command_copy[strlen(command_copy) - 1] = '\0';
	num_of_args = NumOfArgInCommand(command_copy);//returns one if there are no spaces - one word
	token = strtok(command_copy, " ");
	if(num_of_args>1) //atleast two words
	{
		if(strcmp(token, "message")==0)
		{
			HandleMessageCommand(ui->command, num_of_args, communication->socket);
		}
		else if(strcmp(token, "broadcast")==0)
		{
			HandleBroadcastCommand(ui->command, num_of_args, communication->socket);
		}
		else if(strcmp(token, "play")==0 || strcmp(token, "players")==0) 
			write_log_and_print("Illegal argument for command %s. Command format is %s\n",token, token);
		else
			write_log_and_print("Command %s is not recognized. Possible commands are:players, message, broadcast and play\n", 
			ui->command);
	}
	else if(strcmp(command_copy, "play")==0)
	{
		if (HandlePlayCommand(communication, ui, board) == TRUE)
			return TRUE;
	}
	else if (strcmp(command_copy, "players")==0)
	{	
		SendMessageToServer(communication->socket, ui->command);
	}
	else
		write_log_and_print("Command %s is not recognized. Possible commands are:players, message, broadcast and play\n", 
		ui->command);
	ReleaseSemaphoreSimple(ui->EngineDoneWithUserMessageSemaphore);///check if 
	return FALSE;
}

void HandleMessageCommand(char *command, int num_of_args, SOCKET socket)
{
	char *token;
	if(num_of_args<3)
	{
		write_log_and_print("Illegal argument for command %s. Command format is %s <user> <message>\n",token, token);
		return;
	}
	token = strtok(NULL, " ");
	if(CheckIfUserNameValid(token)==FALSE)
	{
		write_log_and_print("Illegal username\n");
		return;
	}
	token = strtok(NULL, " ");
	if(CheckIfMessageValid(token)==FALSE)
	{
		write_log_and_print("Illegal message\n");
		return;
	}
	SendMessageToServer(socket, command);
}

void HandleBroadcastCommand(char *command, int num_of_args, SOCKET socket)
{
	char *token;
	if(num_of_args<2)
	{
		write_log_and_print("Illegal argument for command %s. Command format is %s <message>\n", token, token);
		return;
	}
	token = strtok(NULL, "\n");
	if(CheckIfMessageValid(token)==FALSE)
	{
		write_log_and_print("Illegal message");
		return;
	}
	SendMessageToServer(socket, command);
}

BOOL HandlePlayCommand(data_communication *communication, data_ui *ui, game_board *board)
{
	int dice_result;
	char message[MAX_MESSAGE_SIZE];
	char broadcast_message[MAX_MESSAGE_SIZE];
	DWORD lock_result = 0;
	BOOL is_game_ended = FALSE;

	lock_result = WaitForSingleObject(ui->PlayersTurnEvent, 0);
	switch(lock_result)
	{
		case WAIT_OBJECT_0:
			break;
		default:
			//Its not the player turn, not playing...
			printf("Not your turn...\n");
			return;
	}

	dice_result = (double)rand() / (RAND_MAX + 1) * (MAX_DICE_VALUE - MIN_DICE_VALUE)  
        + MIN_DICE_VALUE;
	is_game_ended = UpdateBoard(board, communication->game_piece, dice_result); 
	PrintBoard(board);

	sprintf(message, "Player %c (%s) drew a %d.\n", 
		communication->game_piece, communication->username, dice_result);
	printf("%s", message);
	
	sprintf(broadcast_message, "broadcast %s", message);
	SendMessageToServer(communication->socket, broadcast_message);
	if (is_game_ended)
	{
		memset(broadcast_message, '\0', MAX_MESSAGE_SIZE);
		sprintf(broadcast_message, "Player %s won the game. Congratulations.\n",
			communication->username);
		write_log_and_print("%s", broadcast_message);
		SendMessageToServer(communication->socket, broadcast_message);	
	}
	ResetEvent(ui->PlayersTurnEvent);
	return is_game_ended;
}

BOOL HandleServerMessage(data_communication *communication, data_ui *ui, game_board *board)
{
	write_log("Received from server: %s", communication->message);
	printf("%s", communication->message);

	if (strstr(communication->message, "Private message from") == NULL &&
		strstr(communication->message, "Broadcast from") == NULL)
	{
		if(strcmp(communication->message, "Your turn to play.") == 0)
			SetEvent(ui->PlayersTurnEvent);
		else if (strstr(communication->message, "your game piece is") != NULL)
		{
			communication->game_piece = communication->message[strlen(communication->message)-2];
		} 
		else if (strstr(communication->message, "drew a") != NULL)
		{
			char *token = NULL;
			char game_piece;
			int dice_result;
			game_piece = communication->message[7];  //game piece
			dice_result = atoi(&(communication->message[strlen(communication->message) - 2]));

			UpdateBoard(board, game_piece, dice_result);
			PrintBoard(board);
		} 
		else if (strstr(communication->message, "won the game") != NULL)
			return TRUE;
	}
	ReleaseSemaphoreSimple(communication->EngineDoneWithServerMessageSemaphore);
	return FALSE;
}

BOOL SendMessageToServer(SOCKET socket, char *message)
{
	write_log("Send to server:%s", message);
	return write_to_socket(socket, message); 
}

BOOL CheckIfMessageValid(char *message)
{
	int length,i;
	if (message == NULL)
		return FALSE;
	length = strlen(message); //returns not including \0
	if(length>MAX_MESSAGE_SIZE)
		return FALSE;
	else
	{
		for(i=0;i<length;i++)
		{
			if(message[i]!=' ' && message[i]!='.' && message[i]!=',' && isdigit(message[i])==0 && isalpha(message[i])==0)
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CheckIfUserNameValid(char *user_name)
{
	int length,i;
	length = strlen(user_name); //returns not including \0
	if(length>MAX_USER_NAME_LENGTH)
		return FALSE;
	else
	{
		for(i=0;i<length;i++)
		{
			if(isdigit(user_name[i])==0 && isalpha(user_name[i])==0)
				return FALSE;
		}
	}
	return TRUE;
}

int NumOfArgInCommand(char *command)
{
	char *token=NULL;
	int counter=0;
	char command_copy[MAX_COMMAND_LENGTH];
    /* get the first token */
	strcpy(command_copy, command);
    token = strtok(command_copy, " ");
    /* walk through other tokens */
    while( token != NULL ) 
    {
		counter++;
		token = strtok(NULL, " ");
	}
	return counter;
}

void ExitGame(data_communication *communication, data_ui *ui, HANDLE *threads)
{
	LPDWORD exit_code;
	close_log();
	//Close sockets
	while (threads != NULL)
	{
		TerminateThread(*threads, 0); 
		CloseHandle(*threads);
		threads++;
	}
	//Close semaphores
	ForceCloseHandle(communication->EngineDoneWithServerMessageSemaphore);
	ForceCloseHandle(communication->IncomingMessageFromServerSemaphore);
	ForceCloseHandle(ui->EngineDoneWithUserMessageSemaphore);
	ForceCloseHandle(ui->UserEnteredTextSemaphore);
	//Close events
	ForceCloseHandle(ui->PlayersTurnEvent);
	//Close sockets
	if (communication->socket != NULL)
		closesocket(communication->socket);
	//Exit
	exit(GetLastError());
}

void ForceCloseHandle(HANDLE handle)
{
	if (handle != NULL)
		CloseHandle(handle);
}