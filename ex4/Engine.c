#include "Engine.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void RunClient(int port, char *username)
{
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;
	game_board board;
	data_ui ui;
	data_communication communication;
	communication.port = port;
	communication.username = username;
	
	ConnectToServer(&communication);
	
	RunUiThread(&ui);
	RunClientCommunicationThread(&communication);
	mutexes[0] = ui.UserEnteredTextSemaphore;
	mutexes[1] = communication.IncomingMessageFromServerSemaphore;

	if (mutexes[0] == NULL || mutexes[1] == NULL) 
	{ 
		printf("ERROR\n\n");
	}
	
	//init random
	srand(time(NULL));

	BuildBoard(&board);

	while (1) 
	{
		lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:				
				ReceivedUserMessage(&communication, &ui, &board);
				break;
			case WAIT_OBJECT_0 + 1:
				HandleServerMessage(&communication, &ui, &board);
				break;
			default:
				printf("result: 0x%x\n", GetLastError());
				//todo exit
				break;
		}
	}

	//todo remove
	getchar();
}

void ConnectToServer(data_communication *communication) 
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
		}
	}
	else 
	{
		write_log_and_print("Failed connecting to server on port %d\n", communication->port);		
		//todo exit and free all
	}
}

void RunClientCommunicationThread(data_communication *communication) 
{
	HANDLE clientCommunicationHandle = NULL;

	communication->IncomingMessageFromServerSemaphore = 
		CreateSemaphoreSimple("IncomingMessageFromServerSemaphore");
	communication->EngineDoneWithServerMessageSemaphore = 
		CreateSemaphoreSimple("EngineDoneWithServerMessageSemaphore");
	//todo check if semaphore creation failed
	clientCommunicationHandle = CreateThread(NULL, 0, RunClientCommunication, communication, 0, NULL);
	if(clientCommunicationHandle == NULL)
	{
		write_log_and_print("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void RunUiThread(data_ui *ui) 
{
	HANDLE uiHandle = NULL;

	ui->UserEnteredTextSemaphore = 
		CreateSemaphoreSimple("UserEnteredTextSemaphore");
	ui->EngineDoneWithUserMessageSemaphore = 
		CreateSemaphoreSimple("EngineDoneWithUserMessageSemaphore");
	//todo check if semaphore creation failed
	uiHandle = CreateThread(NULL, 0, RunUiManager, ui, 0, NULL);
	if(uiHandle == NULL)
	{
		write_log_and_print("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void ReceivedUserMessage(data_communication *communication, data_ui *ui, game_board *board)
{
	DWORD return_value;
	//todo split to functions
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
		HandlePlayCommand(communication, ui, board);
	}
	else if (strcmp(command_copy, "players")==0)
	{	
		SendMessageToServer(communication->socket, ui->command);
	}
	else
		write_log_and_print("Command %s is not recognized. Possible commands are:players, message, broadcast and play\n", 
		ui->command);
	// if Game ended: close_socket(); 
	ReleaseSemaphoreSimple(ui->EngineDoneWithUserMessageSemaphore);///check if 
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

void HandlePlayCommand(data_communication *communication, data_ui *ui, game_board *board)
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
}

void HandleServerMessage(data_communication *communication, data_ui *ui, game_board *board)
{
	write_log_and_print("Received from server: %s", communication->message);

	if (strstr(communication->message, "Private message from") == NULL &&
		strstr(communication->message, "Broadcast from") == NULL)
	{
		if(strcmp(communication->message, "Your turn to play.")==0)
			SetEvent(ui->PlayersTurnEvent);
		else if (strstr(communication->message, "your game piece is") != NULL)
		{
			communication->game_piece = communication->message[strlen(communication->message)-2];
		} 
		else if (strstr(communication->message, "drew a"))
		{
			char *token = NULL;
			char game_piece;
			int dice_result;
			game_piece = communication->message[7];  //game piece
			dice_result = atoi(&(communication->message[strlen(communication->message) - 2]));

			UpdateBoard(board, game_piece, dice_result);
			PrintBoard(board);
		} 
	}
	//todo handle all messages from the server (broadcast etc)
	ReleaseSemaphoreSimple(communication->EngineDoneWithServerMessageSemaphore);
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
	if(length>80)
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