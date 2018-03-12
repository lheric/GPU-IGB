//
// Sockets.cpp
//

#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <crtdbg.h>
#include "..\Common\dbgnew.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <assert.h>

#include <iostream>
#include <algorithm>
#include "Sockets.h"

using namespace std;

// =======================================================================
// CSocket::Initialize
// =======================================================================
int CSocket::Initialize(void)
{
	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	return 0;
}


CSocket::CSocket() :
	m_ConnectSocket(INVALID_SOCKET),
	m_ListenSocket(INVALID_SOCKET)
{
}

CSocket::CSocket(SOCKET S) :
	m_ConnectSocket(S),
	m_ListenSocket(INVALID_SOCKET)
{
}

CSocket::~CSocket()
{
	closesocket(m_ConnectSocket);
}

// =======================================================================
// CSocket::AcceptTCPConnection
// =======================================================================
CSocket* CSocket::AcceptTCPConnection()
{
	CSocket* new_socket = NULL;

	SOCKET CS = accept(m_ListenSocket, NULL, NULL);
	if (CS == INVALID_SOCKET)
	{
		printf("ErrorCSocket::AcceptTCPConnection -- accept failed with error: %d\n", WSAGetLastError());
		closesocket(m_ListenSocket);
		WSACleanup();
		return NULL;
	}

	new_socket = new CSocket(CS);

	return new_socket;
}

// =======================================================================
// CSocket::StartTCPServer
// =======================================================================
int CSocket::StartTCPServer(char* port)
{
	struct addrinfo *result = NULL;
	struct addrinfo  hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	int iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0)
	{
		printf("Error in CSocket::StartTCPServer -- getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		printf("Error in CSocket::StartTCPServer -- socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}


	iResult = bind(m_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("Error in CSocket::StartTCPServer -- bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(m_ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(m_ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("Error in CSocket::StartTCPServer -- listen failed with error: %d\n", WSAGetLastError());
		closesocket(m_ListenSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}


// =======================================================================
// CSocket::ConnectToTCPServer
// =======================================================================
int CSocket::ConnectToTCPServer(const char* name, const char* port)
{
	struct addrinfo *result = NULL;
	struct addrinfo  hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(name, port, &hints, &result);
	if (iResult != 0)
	{
		printf("Error in CSocket::ConnectToTCPServer -- getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds

	for (struct addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		printf("Creating a SOCKET\n");
		m_ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_ConnectSocket == INVALID_SOCKET)
		{
			printf("Error in CSocket::ConnectToTCPServer -- socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		printf("Waiting to CONNECT to server\n");

		iResult = connect(m_ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(m_ConnectSocket);
			m_ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (m_ConnectSocket == INVALID_SOCKET)
	{
		printf("Error in CSocket::ConnectToTCPServer -- Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	return 0;
}

// =======================================================================
// CSocket::SendTCPMessage
// =======================================================================
int CSocket::SendTCPMessage(CMD_t *cmd, char* payload)
{

	//
	// Send the command header.
	//
	int n = send(m_ConnectSocket, (char*)cmd, sizeof(CMD_t), 0);
	if (n == SOCKET_ERROR)
	{
		cout << "\nCSocket::SendTCPMessage -- send failed with error " << WSAGetLastError();
		return 1;
	}

	if (n != sizeof(CMD_t))
	{
		cout << "\nCSocket::SendTCPMessage -- send failed N != CMD_t " << WSAGetLastError();
		return 1;
	}

	int err = 0;
	unsigned int num_left = cmd->payload_len;
	unsigned int num_sent;
	const char *cp = payload;

	//
	// Send the payload if it exists.
	//
	while (num_left > 0)
	{
		num_sent = send(m_ConnectSocket, cp, num_left, 0);

		if (num_sent == SOCKET_ERROR)
		{
			err = SOCKET_ERROR;
			cout << "\nCSocket::SendTCPMessage -- payload send failed  " << WSAGetLastError();
			break;
		}

		assert(num_sent <= num_left);

		num_left -= num_sent;
		cp += num_sent;
	}

	return (err == SOCKET_ERROR ? SOCKET_ERROR : 0);
}


// =======================================================================
// CSocket::ReceiveTCPMessage
// =======================================================================
int CSocket::ReceiveTCPMessage(CMD_t *cmd, char** payload, unsigned int *payload_size)
{
	int n = recv(m_ConnectSocket, (char*)cmd, sizeof(CMD_t), 0);
	if (n == 0)
		cout << "\nCSocket::ReceiveTCPMessage -- connection closed";
	else
		if (n > 0)
		{
			if (n == sizeof(CMD_t))
			{
				unsigned int total = 0;						// number of bytes we've received
				unsigned int bytesleft = cmd->payload_len;	// number of bytes we have left to receive
				n = 0;

				//
				// If the user supplied us with a buffer then use it.
				//
				char *p = *payload;
				if (p == NULL && bytesleft > 0)
					p = new char[bytesleft];

				while (total < cmd->payload_len)
				{
					n = recv(m_ConnectSocket, p + total, bytesleft, 0);
					if (n <= 0)
						break;
					total += n;
					bytesleft -= n;
				}

				if (n == SOCKET_ERROR)
					cout << "\nCSocket::ReceiveTCPMessage -- payload recv failed with error " << WSAGetLastError();

				*payload = p;
				*payload_size = total;		// return the length of the payload according to the CMD struc

				return (n < 0) ? -1 : 0;	// return -1 on failure, 0 on success
			}
			else
			{
				cout << "\nCSocket::ReceiveTCPMessage -- bytes received not equal to CMD_t size\n";
				return 1;
			}
		}
		else
		{
			cout << "\nCSocket::ReceiveTCPMessage -- recv failed with error " << WSAGetLastError();
			return 1;
		}

	return 0;
}

#if 0
int receiveall(int s, char *buf, int *len)
{
	int total = 0;        // how many bytes we've received
	int bytesleft = *len; // how many we have left to receive
	int n = -1;

	while (total < *len) {
		n = recv(s, buf + total, bytesleft, 0);
		if (n <= 0) { break; }
		total += n;
		bytesleft -= n;
	}

	*len = total; // return number actually received here

	return (n <= 0) ? -1 : 0; // return -1 on failure, 0 on success
}
#endif




