#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <stdio.h>
#include <time.h>
#include "Log.h"
#include "Mutex.h"
#include "MutexConstants.h"
#include "SocketWrapper.h"
#include "ServerCommunication.h"
#include "GameConsts.h"


void start_server(int port);

BOOL wait_for_players(int port, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], 
	char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS], 
	communication_data players_communications[MAX_NUM_OF_PLAYERS], HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS]);

void close_connections(SOCKET user_sockets[MAX_NUM_OF_PLAYERS]);

void write_to_log_order_of_players(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL broadcast_players(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL send_players_to_user(SOCKET user_sock, SOCKET user_sockets[MAX_NUM_OF_PLAYERS], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], char symbols[MAX_NUM_OF_PLAYERS]);

BOOL receive_username(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH]);

BOOL send_welcome_message(SOCKET user_sock, char username[MAX_USER_NAME_LENGTH], char user_symbol);

BOOL broadcast_new_player_joined(SOCKET user_sockets[MAX_NUM_OF_PLAYERS], int index_of_new_player, char new_player_username[MAX_USER_NAME_LENGTH], char new_player_symbol);

BOOL username_exists(char username[MAX_USER_NAME_LENGTH], char users[MAX_NUM_OF_PLAYERS][MAX_USER_NAME_LENGTH], int user_index);

void CloseThreads(HANDLE players_communication_thread[MAX_NUM_OF_PLAYERS]);