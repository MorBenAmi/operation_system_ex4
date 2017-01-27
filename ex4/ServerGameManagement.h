#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <stdio.h>
#include <time.h>
#include "Log.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "MutexConstants.h"
#include "SocketWrapper.h"
#include "ServerCommunication.h"
#include "GameConsts.h"


void start_server(int port);

BOOL WaitForPlayers(int port, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], 
	char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS], 
	communication_data players_communications[MAX_NUM_OF_PLAYERS], HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS],
	HANDLE all_threads_must_end_event);

BOOL PlayGame(char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char symbols[MAX_NUM_OF_PLAYERS], HANDLE all_threads_must_end_event);

void CloseConnections(SOCKET user_sockets[MAX_NUM_OF_PLAYERS]);

void WriteToLogOrderOfPlayers(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL BroadcastPlayers(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL SendPlayersToUser(SOCKET user_sock, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL ReceiveUsername(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH]);

BOOL SendWelcomeMessage(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH], char user_symbol);

BOOL BroadcastNewPlayerJoined(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], int index_of_new_player, char new_player_username[MAX_USER_NAME_LENGTH], char new_player_symbol);

BOOL IsUsernameExists(char username[MAX_USER_NAME_LENGTH], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], int user_index);

void CloseThreads(HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS]);

BOOL ShouldFinishExecution();