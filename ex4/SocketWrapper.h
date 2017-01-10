#ifndef SOCKET_WRAPPER_HEADER
#define SOCKER_WRAPPER_HEADER

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include "windows.h"
#include <winsock2.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")


BOOL init_WSA();

void clean_WSA();

BOOL accept_connection(SOCKET listen_sock, SOCKET* accepted_sock);

BOOL sock_listen(int port, SOCKET* listen_sock);

BOOL close_socket(SOCKET sock);

BOOL receive_from_socket(SOCKET sock, char* received_message);

BOOL write_to_socket(SOCKET sock, char* message_to_send);

#endif