#include "SocketWrapper.h"

BOOL init_WSA()
{
	WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
	{
		SetLastError(iResult);
        return FALSE;
	}
	return TRUE;
}

void clean_WSA()
{
	WSACleanup();
}

BOOL sock_listen(int port, int max_connections, SOCKET* listen_sock)
{
	SOCKADDR_IN listen_addr;
	u_long nonblockingMode = 1;
	int iResult = 0;
	
	*listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*listen_sock == INVALID_SOCKET)
	{
		SetLastError(WSAGetLastError());
		return FALSE;
	}

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(port);
	listen_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(*listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
	{
		SetLastError(WSAGetLastError());
		return FALSE;
	}

	if(listen(*listen_sock,max_connections) == SOCKET_ERROR)
	{
		SetLastError(WSAGetLastError());
		return FALSE;
	}

	iResult = ioctlsocket(*listen_sock, FIONBIO, &nonblockingMode);
	if (iResult != NO_ERROR)
	  return FALSE;

	return TRUE;
}

void close_socket(SOCKET sock)
{
	closesocket(sock);
}

BOOL accept_connection(SOCKET listen_socket, SOCKET* accepted_socket)
{
	struct sockaddr_in connect_socket_addr;
	int addr_len;
	*accepted_socket = INVALID_SOCKET;

	addr_len = sizeof(connect_socket_addr);
	*accepted_socket = accept(listen_socket, (struct sockaddr*)&connect_socket_addr, &addr_len);
	if(*accepted_socket == INVALID_SOCKET)
	{
		if(WSAGetLastError() == WSAEWOULDBLOCK)
		{
			//no waiting socket.. return true but not socket was accepted!
			WSASetLastError(0);
			return TRUE;
		}
		SetLastError(WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL receive_from_socket(SOCKET sock, char* received_message)
{
	return TRUE;
}

BOOL write_to_socket(SOCKET sock, char* message_to_send)
{
	return TRUE;
}

BOOL connect_socket(int port ,SOCKET* socket_client)
{
	SOCKADDR_IN clientService;
	
	*socket_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*socket_client == INVALID_SOCKET)
	{
		SetLastError(WSAGetLastError());
		return FALSE;
	}

	clientService.sin_family = AF_INET;
	clientService.sin_port = htons(port);
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");

	if ( connect(*socket_client, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
        SetLastError(WSAGetLastError());
		WSACleanup();
        return FALSE;
    }

	return TRUE;
}
