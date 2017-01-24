#ifndef ENGINE_HEADER
#define ENGINE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include "SocketWrapper.h"
#include <windows.h>
#include "UiManager.h"
#include "ClientCommunication.h"
#include "Log.h"
#include "Mutex.h"
#include "Semaphore.h"
#define SIZE_OF_USERNAME 30
#define SIZE_OF_USERNAME_MESSAGE 41
#define MESSAGE_MAX_SIZE 80

void RunClientCommunicationThread(data_communication *communication);

void RunUiThread(data_ui *ui);

void ReceivedUserMessage(data_ui *ui);

void HandleUserCommand(char *command);

void HandleServerMessage(data_communication *communication,data_ui *ui);

void ConnectToServer(data_communication *communication);

BOOL SendMessageToServer(SOCKET socket, char *message);

int NumOfArgInCommand(char *command);

BOOL CheckIfMessageValid(char *message);

BOOL CheckIfUserNameValid(char *user_name);

void RunClient();

#endif