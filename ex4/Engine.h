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

void RunClientCommunicationThread(data_communication *communication);

void RunUiThread(data_ui *ui);

void ReceivedUserMessage(data_communication *communication, data_ui *ui, board *_board);

void HandleServerMessage(data_communication *communication, data_ui *ui, board *_board);

void ConnectToServer(data_communication *communication);

BOOL SendMessageToServer(SOCKET socket, char *message);

int NumOfArgInCommand(char *command);

BOOL CheckIfMessageValid(char *message);

BOOL CheckIfUserNameValid(char *user_name);

void RunClient();

#endif