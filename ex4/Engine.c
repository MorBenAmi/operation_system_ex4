#include "Engine.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void RunClient(int port, char *username)
{
	HANDLE mutexes[2]={NULL};
	DWORD lock_result;
	board _board;
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

	BuildBoard(&_board);

	while (1) 
	{
		lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:				
				ReceivedUserMessage(&communication, &ui, &_board);
				break;
			case WAIT_OBJECT_0 + 1:
				HandleServerMessage(&communication, &ui, &_board);
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
		printf("sending %s\\n\n", username_message);

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
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
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
		printf("Failed to create thread - Error code: 0x%x\n", GetLastError());
		write_log("Failed to create thread - Error code: 0x%x\n", GetLastError());
		return;
	}
}

void ReceivedUserMessage(data_communication *communication, data_ui *ui, board *_board)
{
	DWORD return_value;
	//todo split to functions
	char *token=NULL;
	int num_of_args;
	char command_copy[MAX_COMMAND_LENGTH];
	strcpy(command_copy, ui->command);
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
	}
	else if(strcmp(ui->command, "playy")==0)
	{
		HandlePlayCommand(communication, ui, _board);
	}
	else if (strcmp(ui->command, "players")==0)
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
	if(num_of_args!=3)
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
	if(num_of_args!=2)
	{
		write_log_and_print("Illegal argument for command %s. Command format is %s <message>\n",token, token);
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

void HandlePlayCommand(data_communication *communication, data_ui *ui, board *_board)
{
	int dice_result;
	dice_result = (double)rand() / (RAND_MAX + 1) * (MAX_DICE_VALUE - MIN_DICE_VALUE)  
        + MIN_DICE_VALUE;
	UpdateBoard(_board, communication->game_piece, dice_result); 
	PrintBoard(_board);
	//to do rand() print message and broadcast

	ResetEvent(ui->PlayersTurnEvent);
}

void HandleServerMessage(data_communication *communication, data_ui *ui, board *_board)
{
	write_log_and_print("Received from server: %s\\n\n", communication->message);

	if(strcmp(communication->message,"Your turn to play")==0)
		SetEvent(ui->PlayersTurnEvent);
	else if (strstr(communication->message, "your game piece is") != NULL)
	{
		char *token = NULL;
		token = strtok(communication->message, "your game piece is");
		strcpy(communication->username, token);
		token = strtok(NULL, "your game piece is");
		communication->game_piece = *token;
	}
	//todo handle all messages from the server (broadcast etc)
	ReleaseSemaphoreSimple(communication->EngineDoneWithServerMessageSemaphore);
}

BOOL SendMessageToServer(SOCKET socket, char *message)
{
	write_log("Send to server:%s\\n\n", message);
	//todo: tal, i think it's better to add the \n from outside..because strcat does not validate that you have anough space for appending this \n in message
	strcat(message, "\n");
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