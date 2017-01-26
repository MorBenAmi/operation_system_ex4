#include "Engine.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void RunClient(int port, char *username)
{
	HANDLE semaphores[NUMBER_OF_THREADS];
	HANDLE threads[NUMBER_OF_THREADS];
	DWORD lock_result;
	game_board board;
	data_ui ui;
	data_communication communication;
	int i;

	//Init port and username
	communication.port = port;
	communication.username = username;
	//Set communication flag
	communication.communication_error = FALSE;
	//Set all handles to NULL
	communication.EngineDoneWithServerMessageSemaphore = NULL;
	communication.IncomingMessageFromServerSemaphore = NULL;
	//Set all handles to NULL
	ui.EngineDoneWithUserMessageSemaphore = NULL;
	ui.UserEnteredTextSemaphore = NULL;
	ui.PlayersTurnEvent = NULL;
	//Init all threads to NULL
	for (i = 0; i < NUMBER_OF_THREADS; i++)
	{
		threads[i] = NULL;
		semaphores[i] = NULL;
	}

	//Connect to the server
	if (ConnectToServer(&communication) == FALSE) 
		ExitGame(&communication, &ui, threads);
	
	threads[0] = RunUiThread(&ui);
	if (threads[0] == NULL)
		ExitGame(&communication, &ui, threads);

	threads[1] = RunClientCommunicationThread(&communication);
	if (threads[1] == NULL)
		ExitGame(&communication, &ui, threads);
	
	semaphores[0] = ui.UserEnteredTextSemaphore;
	semaphores[1] = communication.IncomingMessageFromServerSemaphore;
	
	//init random
	srand(time(NULL));
	//Builds the board
	BuildBoard(&board);

	while (1) 
	{
		lock_result = (WaitForMultipleObjects(NUMBER_OF_THREADS, semaphores, FALSE, INFINITE));
		switch (lock_result)
		{
			case WAIT_OBJECT_0:				
				//If thread 0 finished, receive user msg
				if (ReceivedUserMessage(&communication, &ui, &board) == TRUE)
					ExitGame(&communication, &ui, threads);
				break;
				//If thread 1 finished, receive server msg
			case WAIT_OBJECT_0 + 1:
				if (HandleServerMessage(&communication, &ui, &board) == TRUE)
					ExitGame(&communication, &ui, threads);
				//Error occured while reading from the server
				if (communication.communication_error == TRUE)
					ExitGame(&communication, &ui, threads);
				break;
		}
	}
	ExitGame(&communication, &ui, threads);
}

// Connects to the server using socket. Return TRUE if succeed
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

		return SendMessageToServer(communication->socket, username_message);
	}
	else 
	{
		write_log_and_print("Failed connecting to server on port %d\n", communication->port);		
		return FALSE;
	}
}

//Creates the semaphore that related to the client communication thread. 
//Starts the client communication thread. return TRUE if succeed
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

//Creates the semaphore that related to the ui thread. Runs the HANDLE of the thread
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

//Handles a message from the user. Returns TRUE if the game ended
BOOL ReceivedUserMessage(data_communication *communication, data_ui *ui, game_board *board)
{
	DWORD return_value;
	char *token = NULL;
	int num_of_args;
	char command_copy[MAX_COMMAND_LENGTH];

	strcpy(command_copy, ui->command);
	command_copy[strlen(command_copy) - 1] = '\0';
	num_of_args = NumOfArgInCommand(command_copy);//returns one if there are no spaces - one word
	token = strtok(command_copy, " ");
	if(num_of_args > 1) //atleast two words
	{
		if(strcmp(token, COMMAND_MESSAGE) == 0)
			HandleMessageCommand(ui->command, num_of_args, communication->socket);
		else if(strcmp(token, COMMAND_BROADCAST) == 0)
			HandleBroadcastCommand(ui->command, num_of_args, communication->socket);
		else if(strcmp(token, COMMAND_PLAY) == 0 || strcmp(token, COMMAND_PLAYERS) == 0) 
			write_log_and_print("Illegal argument for command %s. Command format is %s\n", 
			token, token);
		else
			write_log_and_print("Command %s is not recognized. Possible commands are: players, message, broadcast and play.\n", 
			ui->command);
	}
	else if(strcmp(command_copy, COMMAND_PLAY) == 0)
	{
		if (HandlePlayCommand(communication, ui, board) == TRUE)
			return TRUE;
	}
	else if (strcmp(command_copy, COMMAND_PLAYERS) == 0)
	{	
		SendMessageToServer(communication->socket, ui->command);
	}
	else
		write_log_and_print("Command %s is not recognized. Possible commands are: players, message, broadcast and play.\n", 
		ui->command);
	//Release the semaphore that engine done with user msgs
	ReleaseSemaphoreSimple(ui->EngineDoneWithUserMessageSemaphore); 
	return FALSE;
}

//Handles a message command from the user
void HandleMessageCommand(char *command, int num_of_args, SOCKET socket)
{
	char *token = NULL;
	if(num_of_args < COMMAND_MESSAGE_MIN_ARGS)
	{
		write_log_and_print("Illegal argument for command %s. Command format is %s <user> <message>\n",token, token);
		return;
	}
	token = strtok(NULL, " ");
	if(token == NULL)
	{
		write_log_and_print("Illegal username\n");
		return;
	}
	token = strtok(NULL, " ");
	if(CheckIfMessageValid(token) == FALSE)
	{
		write_log_and_print("Illegal message\n");
		return;
	}
	SendMessageToServer(socket, command);
}

//Handles a braodcast message from the user
void HandleBroadcastCommand(char *command, int num_of_args, SOCKET socket)
{
	char *token = NULL;
	if(num_of_args < COMMAND_BROADCAST_MIN_ARGS)
	{
		write_log_and_print("Illegal argument for command %s. Command format is %s <message>\n", token, token);
		return;
	}
	token = strtok(NULL, "\n");
	if(CheckIfMessageValid(token) == FALSE)
	{
		write_log_and_print("Illegal message");
		return;
	}
	SendMessageToServer(socket, command);
}

//Handles play command from the user. Plays only if its his turn. returns TRUE if the game ended
BOOL HandlePlayCommand(data_communication *communication, data_ui *ui, game_board *board)
{
	int dice_result;
	char message[MAX_COMMAND_LENGTH];
	char broadcast_message[MAX_COMMAND_LENGTH];
	DWORD lock_result;
	BOOL is_game_ended = FALSE;

	lock_result = WaitForSingleObject(ui->PlayersTurnEvent, 0);
	switch(lock_result)
	{
		case WAIT_OBJECT_0:
			break;
		default:
			//Its not the player turn, not playing...
			printf("Not your turn...\n");
			return FALSE;
	}

	memset(message, '\0', MAX_COMMAND_LENGTH);
	memset(broadcast_message, '\0', MAX_COMMAND_LENGTH);

	//Rolls the dice to random number between 1 - 6
	dice_result = (double)rand() / (RAND_MAX + 1) * (MAX_DICE_VALUE - MIN_DICE_VALUE)  
        + MIN_DICE_VALUE;
	//Updating the board
	is_game_ended = UpdateBoard(board, communication->game_piece, dice_result); 
	PrintBoard(board);

	sprintf(message, "Player %c (%s) drew a %d.\n", 
		communication->game_piece, communication->username, dice_result);
	printf("%s", message);
	
	sprintf(broadcast_message, "broadcast %s", message);
	//Updates the server about the play
	SendMessageToServer(communication->socket, broadcast_message);

	if (is_game_ended == TRUE)
	{
		memset(broadcast_message, '\0', MAX_COMMAND_LENGTH);
		sprintf(broadcast_message, "Player %s won the game. Congratulations.\n",
			communication->username);
		write_log_and_print("%s", broadcast_message);
		SendMessageToServer(communication->socket, broadcast_message);	
	}
	ResetEvent(ui->PlayersTurnEvent);
	return is_game_ended;
}

//Handles the msgs received from the server. returns TRUE if the game ended
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
			communication->game_piece = 
				communication->message[strlen(communication->message) - GAME_PIECE_POSITION_FROM_END];
		} 
		else if (strstr(communication->message, "drew a") != NULL)
		{
			char *token = NULL;
			char game_piece;
			int dice_result;

			game_piece = communication->message[GAME_PIECE_POSITION_IN_DREW_COMMAND];  //game piece
			dice_result = atoi(&(communication->message[strlen(communication->message) - DICE_RESULT_POSITION]));
			//Updates the board on the user turn
			UpdateBoard(board, game_piece, dice_result);
			PrintBoard(board);
		} 
		else if (strstr(communication->message, "won the game") != NULL)
			return TRUE;
	}
	//Finish handling the message from the server
	ReleaseSemaphoreSimple(communication->EngineDoneWithServerMessageSemaphore);
	return FALSE;
}

//Sends a message to the server and prints to the log.
BOOL SendMessageToServer(SOCKET socket, char *message)
{
	BOOL result;
	write_log("Sent to server: %s", message);
	result = write_to_socket(socket, message); 
	if (result == FALSE)
	{
		SetLastError(WSAGetLastError());
		write_log_and_print("Error while trying to write data to socket. Error code: 0x%x\n", GetLastError());
	}
	return result;
}

//Check is the message is valid. 
//Should contain only digits, chars or ' ', ',', '.'.
//Should be less than 80 chars
BOOL CheckIfMessageValid(char *message)
{
	int length,i;
	if (message == NULL)
		return FALSE;
	length = strlen(message); //returns not including \0
	if(length > MAX_USER_MESSAGE_LENGTH)
		return FALSE;
	else
	{
		for(i=0; i < length; i++)
		{
			if(message[i] != ' ' && 
				message[i] != '.' && 
				message[i] != ',' && 
				isdigit(message[i])==0 &&
				isalpha(message[i])==0)
				return FALSE;
		}
	}
	return TRUE;
}

//Returns the num of args in the command. Splitting by space.
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

//Closes all the HANDLE and socket and exits the program
void ExitGame(data_communication *communication, data_ui *ui, HANDLE *threads)
{
	LPDWORD exit_code;
	int i;
	close_log();
	//Close sockets
	for (i = 0; i < NUMBER_OF_THREADS; i++)
	{
		if (threads[i] == NULL)
			continue;
		TerminateThread(threads[i], 0); 
		CloseHandle(threads[i]);
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

//Close all the handles
void ForceCloseHandle(HANDLE handle)
{
	if (handle != NULL)
		CloseHandle(handle);
}