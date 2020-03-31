// Sydney Caldwell 100652057
//Dylan Moore 100662175

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")

#include <cstdio>
#include <thread>

#define MAX_CLIENTS 4

bool serverRunning = false;


int main()
{
	//Initialise winsock
	WSAData wsaData = {};
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error != 0) {
		fprintf(stderr, "Failed to initialise winsock!\nError code: %ld\n", error);
	}

	//Open a tcp socket to listen for connections.
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//If the socket failed to open, print the error and quit.
	if (serverSocket == INVALID_SOCKET) {
		fprintf(stderr, "Failed to open socket!\nWSAError: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//Prompt the user for a port to bind to:
	unsigned short port = 0;
	int num = 0;
	do {
		printf("Please enter a port to bind to:\n");
		num = scanf("%hu", &port);
	} while (num != 1);



	sockaddr_in addrinfo = {};
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.s_addr = INADDR_ANY;
	addrinfo.sin_port = port;

	//Print error and exit application if the socket fails to bind.
	if (bind(serverSocket, (sockaddr*)(&addrinfo), sizeof(sockaddr_in)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind socket to port '%hd'!\nWSAError: %ld\n", port, WSAGetLastError());

		//Close serverSocket.
		closesocket(serverSocket);

		WSACleanup();
		return 1;
	}

	//Start listeneing for clients.
	//Stop the application if this fails, and print the error.
	if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to listen!\nWSAError: %ld\n", WSAGetLastError());

		//Close serverSocket.
		closesocket(serverSocket);

		WSACleanup();
		return 1;
	}

	serverRunning = true;


	//IP for accept
	sockaddr_in addrinfo = {};
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.s_addr = INADDR_ANY;
	addrinfo.sin_port = 0;

	//Namelen for accept
	int namelen = sizeof(sockaddr_in);

	//Thread to listen for new clients.
	std::thread listenThread = std::thread([&serverSocket, &addrinfo, &namelen]() {
		while (serverRunning) {
			SOCKET client = accept(serverSocket, (sockaddr*)(&addrinfo), &namelen);

			if (client == INVALID_SOCKET) {
				fprintf(stderr, "Failed to accept incoming connection!\nWSAError: %ld\n", WSAGetLastError());
			}
			else {
				//Opened connection. Set client socket to non-blocking mode.
				//Close it if it fails.
				unsigned long iMode = 1;
				int iResult = ioctlsocket(client, FIONBIO, &iMode);
				if (iResult != NO_ERROR) {
					fprintf(stderr, "ioctlsocket failed with error: %ld\n", iResult);
					shutdown(client, SD_BOTH);
					closesocket(client);
					continue;
				}

				sockaddr_in clientaddr = {};
				int clientnamelen = sizeof(sockaddr_in);
				
				//Shutdown the socket if getpeername fails.
				if (getpeername(client, (sockaddr*)& clientaddr, &clientnamelen) == SOCKET_ERROR) {
					fprintf(stderr, "getpeername failed!\nWSAError: %ld\n", WSAGetLastError());
					shutdown(client, SD_BOTH);
					closesocket(client);
					continue;
				}
				char buf[24] = {};
				const char* ipaddr = inet_ntop(clientaddr.sin_family, (sockaddr*)(&clientaddr), buf, 24);
				printf("\nClient connected!\nIP: %s\nPort: %hd\n\n", ipaddr, clientaddr.sin_port);
			}
		}
	});

	//Run thread asnychronously.
	listenThread.detach();
	
	//Run game server.
	while (serverRunning) {

	}

	serverRunning = false;

	//Stop communication over serverSocket. (Both ways: in and out)
	shutdown(serverSocket,SD_BOTH);

	//Close serverSocket.
	closesocket(serverSocket);

	WSACleanup();
	return 0;
}