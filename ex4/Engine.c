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
	
	BuildBoard(&_board);
	PrintBoard(&_board);

	while (1) 
	{
		lock_result = (WaitForMultipleObjects(2, mutexes, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:				
				ReceivedUserMessage(&ui, &_board);
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

void ReceivedUserMessage(data_ui *ui, board *_board)
{
	DWORD return_value;
	Sleep(1000); //todo remove - only for debug
	HandleUserCommand(ui->command);
	if(strcmp(ui->command,"play")==0)
	{
		return_value = WaitForSingleObject(ui->PlayersTurnEvent, 0);
		if (return_value == WAIT_TIMEOUT)
		{

		}
		else
		{
			//to do rand() print message and broadcast

			ResetEvent(ui->PlayersTurnEvent);
		}
	}
	ReleaseSemaphoreSimple(ui->EngineDoneWithUserMessageSemaphore);///check if 
}

void HandleUserCommand(char *command)
{
	//todo validate and handle message
	//if ilegal arg
	char *token=NULL;
	int num_of_arg_in_command;
	num_of_arg_in_command = NumOfArgInCommand(command);//returns zero if there are no spaces - one word
	token = strtok(command, " ");
	if(num_of_arg_in_command!=0) //two words
	{
		if(strcmp(token, "message")==0)
		{
			if(num_of_arg_in_command!=3)
			{
				write_log_and_print("Illegal argument for command %s. Command format is %s <user> <message>",token, token);
				return;
			}
			token = strtok(NULL, " ");
			if(CheckIfUserNameValid(token)==FALSE)
				write_log_and_print("Illegal username");
			token = strtok(NULL, " ");
			if(CheckIfMessageValid(token)==FALSE)
				write_log_and_print("Illegal message");
		}
		else if(strcmp(token, "broadcast")==0)
		{
			if(num_of_arg_in_command!=2)
				write_log_and_print("Illegal argument for command %s. Command format is %s <message>",token, token);
			token = strtok(NULL, " ");
			if(CheckIfMessageValid(token)==FALSE)
				write_log_and_print("Illegal message");
		}
		else if(strcmp(token, "play")==0)
				write_log_and_print("Illegal argument for command %s. Command format is %s",token, token);
		else if(strcmp(token, "players")==0)
				write_log_and_print("Illegal argument for command %s. Command format is %s",token, token);
	}
	else if(strcmp(command, "play")!=0 && strcmp(command, "players")!=0)
	{
		token = command;	
		write_log_and_print("Command %s is not recognized. Possible commands are:players, message, broadcast and play", 
		token);
	}
	// if Game ended: close_socket(); 
}

void HandleServerMessage(data_communication *communication, data_ui *ui, board *_board)
{
	write_log_and_print("Received from server: %s\\n\n", communication->message);
	Sleep(1000); //todo remove - only for debug
	if(strcmp(communication->message,"Your turn to play")==0)
		SetEvent(ui->PlayersTurnEvent);
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
   /* get the first token */
   token = strtok(command, " ");
   /* walk through other tokens */
   while( token != NULL ) 
   {
      counter++;
      token = strtok(NULL, " ");
   }
   return counter;
}