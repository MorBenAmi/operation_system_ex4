#ifndef ENGINE_HEADER
#define ENGINE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SocketWrapper.h"
#include <windows.h>
#include "UiManager.h"
#include "ClientCommunication.h"
#include "Log.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "GameConsts.h"
#include "Board.h"

#define MIN_DICE_VALUE 1
#define MAX_DICE_VALUE 6
#define NUMBER_OF_THREADS 2
#define GAME_PIECE_POSITION_FROM_END 2
#define DICE_RESULT_POSITION 2
#define GAME_PIECE_POSITION_IN_DREW_COMMAND 7

HANDLE RunClientCommunicationThread(data_communication *communication);

HANDLE RunUiThread(data_ui *ui);

BOOL ReceivedUserMessage(data_communication *communication, data_ui *ui, game_board *board);

BOOL HandleServerMessage(data_communication *communication, data_ui *ui, game_board *board);

BOOL ConnectToServer(data_communication *communication);

BOOL SendMessageToServer(SOCKET socket, char *message);

int NumOfArgInCommand(char *command);

BOOL CheckIfMessageValid(char *message);

void HandleMessageCommand(char *command, int num_of_args, SOCKET socket);

void HandleBroadcastCommand(char *command, int num_of_args, SOCKET socket);

BOOL HandlePlayCommand(data_communication *communication, data_ui *ui, game_board *board);

void RunClient(int port, char *username);

void ExitGame(data_communication *communication, data_ui *ui, HANDLE *threads);

void ForceCloseHandle(HANDLE handle);

#endif