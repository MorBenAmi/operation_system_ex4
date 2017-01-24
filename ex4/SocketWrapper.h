#ifndef SOCKET_WRAPPER_HEADER
#define SOCKER_WRAPPER_HEADER

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")

#define SRV_ADDRESS "127.0.0.1"

BOOL init_WSA();

void clean_WSA();

BOOL accept_connection(SOCKET listen_sock, SOCKET* accepted_sock);

BOOL sock_listen(int port, int max_connections, SOCKET* listen_sock);

void close_socket(SOCKET sock);

BOOL receive_from_socket(SOCKET sock, char* received_message);

BOOL write_to_socket(SOCKET sock, char* message_to_send);

BOOL connect_socket(int port, SOCKET* socket);

#endif