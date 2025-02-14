#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFLEN 512
#define PORT 27015
#define ADDRESS "127.0.0.1"

int main() {
	printf("Hello, world!\n");

	int res, sendRes;
	// ===================================================================
	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res) {
		printf("Startup failed: %d\n", res);
		return 1;
	}
	// ===================================================================

	// ===================================================================
	SOCKET listener;
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == INVALID_SOCKET) {
		printf("Error with construction: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ADDRESS);
	address.sin_port = htons(PORT);
	bind(listener, (struct sockaddr*)&address, sizeof(address));
	if (res == SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(listener);
		WSACleanup();
		return 1;
	}

	res = listen(listener, SOMAXCONN);
	
	
	if (res == SOCKET_ERROR) {
		printf("Listen failed: %d\n", WSAGetLastError());
		closesocket(listener);
		WSACleanup();
		return 1;
	}

	printf("Accepting on %s:%d\n", ADDRESS, PORT);
	// ===================================================================

	// ===================================================================
	SOCKET client;
	struct sockaddr_in clientAddr;
	int clientAddrlen;
	client = accept(listener, NULL, NULL);
	if (client == INVALID_SOCKET) {
		printf("Could not accept: %d\n", WSAGetLastError());
		closesocket(listener);
		WSACleanup();
		return 1;
	}

	getpeername(client, (struct sockaddr *) &clientAddr, &clientAddrlen);
	printf("Client connected at %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	char *welcome = "Welcome to the server!";
	sendRes = send(client, welcome, strlen(welcome), 0);
	if (sendRes != strlen(welcome)) {
		printf("Error sending: %d\n", WSAGetLastError());
		shutdown(client, SD_BOTH);
		closesocket(client);
	}

	char recvbuf[BUFFLEN];
	int recvbuflen = BUFFLEN;
	do {
		res = recv(client, recvbuf, recvbuflen, 0);
		if (res > 0) {
			recvbuf[res] = '\0';
			printf("Message recieved (%d): %s\n", res, recvbuf);

			if (!memcmp(recvbuf, "/quit", 5 * sizeof(char))) {
				printf("Closing connection.\n");
				break;
			}
		}
		else if (!res) {
			printf("Closing connection.\n");
			break;
		}
		else {
			printf("Recieve failed: %d\n", WSAGetLastError());
			break;
		}
	} while (res > 0);

	res = shutdown(client, SD_BOTH);
	if (res == SOCKET_ERROR) {
		printf("Client shutdown failed: %d\n", WSAGetLastError());
	}
	closesocket(client);

	// ===================================================================

	// ===================================================================
	closesocket(listener);

	res = WSACleanup();
	if (res) {
		printf("Cleanup failed: %d\n", res);
		return 1;
	}

	printf("Shutting down. \nGood night.\n");
	// ===================================================================
	return 0;
}