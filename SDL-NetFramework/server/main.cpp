// Sydney Caldwell 100652057
//Dylan Moore 100662175

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")

#include <cstdio>
#include <chrono>
#include <thread>
#include <vector>

#define MAX_CLIENTS 8
#define TICK_DELAY 250//16

bool serverRunning = false;

struct vec2 {
	float x = 0.0f;
	float y = 0.0f;
};

struct Client {
	sockaddr_in address = {};
	bool positionChanged = false;
	vec2 positionUpdate = {};
	const char* name;
};


std::vector<Client> clientList = std::vector<Client>();

int main()
{
	printf("Enter max number of clients:\n > ");
	int maxClients = 0;
	while (1) {
		if (scanf_s("%d", &maxClients) == 1) {
			break;
		}
		printf("Error: Please enter a number!\n");
		printf("Enter max number of clients:\n > ");
	}

	//Prompt the user for a port to bind to:
	printf("Enter port to bind to:\n > ");
	unsigned short port = 0;
	while (1) {
		if (scanf_s("%hu", &port) == 1) {
			break;
		}
		printf("Error: Please enter a valid port!\n");
		printf("Enter max number of clients:\n > ");
	}

	printf("Starting server...\n");
	//Initialise winsock
	WSAData wsaData = {};
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error != 0) {
		fprintf(stderr, "Failed to initialise winsock!\nError code: %ld\n", error);
	}

	//Open a tcp socket to listen for connections.
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Open a udp socket to send updates.
	SOCKET udpSock = INVALID_SOCKET;
	udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//If the sockets failed to open, print the error and quit.
	if (serverSocket == INVALID_SOCKET) {
		fprintf(stderr, "Failed to open tcp socket!\nWSAError: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	if (udpSock == INVALID_SOCKET) {
		fprintf(stderr, "Failed to open udp socket!\nWSAError: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	
	sockaddr_in bindaddrinfo = {};
	bindaddrinfo.sin_family = AF_INET;
	bindaddrinfo.sin_addr.s_addr = INADDR_ANY;
	bindaddrinfo.sin_port = htons(port);


	printf("Binding sockets...\n");
	//Print error and exit application if the sockets fail to bind.
	if (bind(udpSock, (sockaddr*)(&bindaddrinfo), sizeof(sockaddr_in)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind udp socket to port '%hd'!\nWSAError: %ld\n", port, WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}
	if (bind(serverSocket, (sockaddr*)(&bindaddrinfo), sizeof(sockaddr_in)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind tcp socket to port '%hd'!\nWSAError: %ld\n", port, WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}
	

	printf("Listening for clients...\n");
	//Start listeneing for clients.
	//Stop the application if this fails, and print the error.
	if (listen(serverSocket, maxClients) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to listen!\nWSAError: %ld\n", WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}

	serverRunning = true;
	printf("Server started.\n");

	//IP for accept
	sockaddr_in addrInfo = {};
	addrInfo.sin_family = AF_INET;
	addrInfo.sin_addr.s_addr = INADDR_ANY;
	addrInfo.sin_port = 0;

	//Namelen for accept
	int namelen = sizeof(sockaddr_in);

	//Thread to listen for new clients.
	std::thread listenThread = std::thread([&serverSocket, &addrInfo, &namelen, &udpSock]() {
		printf("Starting listenThread...\n");
		while (serverRunning) {
			SOCKET client = accept(serverSocket, (sockaddr*)(&addrInfo), &namelen);
			printf("Incoming connection request...\n");

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

				//Client has connected. Print it to the console.
				char buf[24] = {};
				const char* ipaddr = inet_ntop(AF_INET, (sockaddr*)(&clientaddr), buf, 24);
				printf("\nClient connected!\nIP: %s\nPort: %hd\n\n", ipaddr, clientaddr.sin_port);

				//Add to list of clients
				Client client = {};
				client.address = clientaddr;
				client.name = ipaddr;
				client.positionChanged = 0;
				client.positionUpdate = {};

				clientList.push_back(client);
			}
		}
	});

	//Run thread asnychronously.
	listenThread.detach();

	
	//Run game server.
	std::chrono::time_point<std::chrono::steady_clock>	lastClock = std::chrono::steady_clock::now(), now=std::chrono::steady_clock::now();
	while (serverRunning) {
		//Receive messages
		{
			char buf[512];
			printf("Clear client list.\n");
			for (int i = 0; i < clientList.size(); ++i) {
				clientList[i].positionChanged = 0;
			}
			for (int i = 0; i < clientList.size(); ++i) {
				memset(buf, 0, 512);
				int namelen = sizeof(sockaddr_in);
				int received = recvfrom(udpSock, buf, 512, 0, (sockaddr*)&clientList[i].address, &namelen);
				//If no error occured...
				if (received != SOCKET_ERROR) {
					printf("RECV from %s: %s", clientList[i].name, buf);
					printf("\n");
				}
				//If the packet has contents...
				if (received > 0) {
					//Try and parse it
					if (sscanf_s(buf, "%f %f", &clientList[i].positionUpdate.x, &clientList[i].positionUpdate.y)==2) {
						//Set position as changed if successfully read.
						clientList[i].positionChanged = 1;
						printf("Position received: %f, %f\n", clientList[i].positionUpdate.x, clientList[i].positionUpdate.y);
					}
					
				}
			}
		}
		//Wait until it is time to send the packet.
		lastClock = now;
		while (std::chrono::duration_cast<std::chrono::milliseconds>(now-lastClock)<std::chrono::milliseconds(TICK_DELAY)) {
			now = std::chrono::steady_clock::now();
		}


		//Create a buffer to hold the packet data.
		char buf[512] = {};
		memset(buf, 0, 512);

		size_t index = 0;
		index = sprintf_s(buf, size_t(512), "%d\n", (int)clientList.size());


		//Loop through and do the same for all the client info.
		for (int i = 0; i < clientList.size(); ++i) {
			//Shorthand for clientList[i]
			const Client& client = clientList[i];
			printf("Client %d: %f %f\n", i, client.positionUpdate.x, client.positionUpdate.y);

			//Identify the client by its index
			index += sprintf_s(buf+index, size_t(512) - index, "%d ", i);

			//Is a position provided (Did the position change?)
			index += sprintf_s(buf + index, size_t(512) - index, "%d ", client.positionChanged);

			//If so, stick the new position in there too.
			if (client.positionChanged) {
				index += sprintf_s(buf + index, size_t(512) - index, "%f %f\n", client.positionUpdate.x, client.positionUpdate.y);
			}
		}

		printf("SENDING: %s\n", buf);
		//Send the packet update.
		for (Client client : clientList) {
			printf("SENDING TO %s: %s\n", client.name, buf);
			sendto(udpSock, buf, strlen(buf), 0, (sockaddr*)&client.address, namelen);
		}

		//Exit on "Q"
		if (GetAsyncKeyState(0x51)) {
			serverRunning = false;
		}
	}

	serverRunning = false;

	//Stop communication over all sockets. (Both ways: in and out)
	shutdown(serverSocket, SD_BOTH);
	shutdown(udpSock, SD_BOTH);

	//Close sockets.
	closesocket(serverSocket);
	closesocket(udpSock);

	WSACleanup();
	return 0;
}