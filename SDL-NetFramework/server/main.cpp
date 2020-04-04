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
#define TICK_DELAY 16

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


std::vector<Client> clientListA = std::vector<Client>();
std::vector<Client> clientListB = std::vector<Client>();

std::vector<Client> *clientListRead = &clientListA, *clientListWrite = &clientListB;

void clientListSwap() {
	std::swap(clientListRead, clientListWrite);
}

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
	bindaddrinfo.sin_port = port;

	//Print error and exit application if the sockets fail to bind.
	if (bind(serverSocket, (sockaddr*)(&bindaddrinfo), sizeof(sockaddr_in)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind tcp socket to port '%hd'!\nWSAError: %ld\n", port, WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}
	if (bind(udpSock, (sockaddr*)(&bindaddrinfo), sizeof(sockaddr_in)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind udp socket to port '%hd'!\nWSAError: %ld\n", port, WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}

	//Start listeneing for clients.
	//Stop the application if this fails, and print the error.
	if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to listen!\nWSAError: %ld\n", WSAGetLastError());

		//Close sockets.
		closesocket(serverSocket);
		closesocket(udpSock);

		WSACleanup();
		return 1;
	}

	serverRunning = true;

	//IP for accept
	sockaddr_in addrInfo = {};
	addrInfo.sin_family = AF_INET;
	addrInfo.sin_addr.s_addr = INADDR_ANY;
	addrInfo.sin_port = 0;

	//Namelen for accept
	int namelen = sizeof(sockaddr_in);

	//Thread to listen for new clients.
	std::thread listenThread = std::thread([&serverSocket, &addrInfo, &namelen, &udpSock]() {
		while (serverRunning) {
			SOCKET client = accept(serverSocket, (sockaddr*)(&addrInfo), &namelen);

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
				const char* ipaddr = inet_ntop(clientaddr.sin_family, (sockaddr*)(&clientaddr), buf, 24);
				printf("\nClient connected!\nIP: %s\nPort: %hd\n\n", ipaddr, clientaddr.sin_port);

				//Add to list of clients
				Client client = {};
				client.address = clientaddr;
				client.name = ipaddr;
				client.positionChanged = 0;
				client.positionUpdate = {};

				clientListWrite->push_back(client);
			}
		}
	});

	//Run thread asnychronously.
	listenThread.detach();
	
	std::thread recvThread = std::thread([&udpSock]() {
		while (serverRunning) {
			char buf[512];
			for (Client client : (*clientListWrite)) {
				client.positionChanged = 0;
			}
			for (Client client : (*clientListWrite)) {
				memset(buf, 0, 512);
				int namelen = sizeof(sockaddr_in);
				int received = recvfrom(udpSock, buf, 512, 0, (sockaddr*)&client.address, &namelen);
				//If no error occured...
				if (received != SOCKET_ERROR) {
					printf("RECV from %s: %s\n", client.name, buf);
				}
				//If the packet has contents...
				if (received > 0) {
					//If the contents are the correct size...
					if (received == sizeof(float) * 2) {
						//Set position update.
						client.positionChanged = 1;
						client.positionUpdate.x = *(&buf[0]);
						client.positionUpdate.y = *(&buf[4]);
					}
				}
			}
			//Swap the double buffer
			clientListSwap();
		}
		});
	recvThread.detach();

	
	//Run game server.
	std::chrono::time_point<std::chrono::steady_clock>	lastClock = std::chrono::steady_clock::now(), now=std::chrono::steady_clock::now();
	while (serverRunning) {
		//Wait until it is time to send the packet.
		lastClock = now;
		while (std::chrono::duration_cast<std::chrono::milliseconds>(now-lastClock)<std::chrono::microseconds(TICK_DELAY)) {
			now = std::chrono::steady_clock::now();
		}

		//Get a constant snapshot of the client list.
		const std::vector<Client> clientList = std::vector<Client>(*clientListRead);


		//Create a buffer to hold the packet data.
		char buf[512] = {};
		memset(buf, 0, 512);

		//Fill in the packet
		int bufIterator = 0;

		//Place the number of clients into the byte array first
		*(int*)((void*)&buf[bufIterator]) = (int)clientList.size();

		//Increment the iterator by the number of bytes put there. (So it isn't overwritten)
		bufIterator += sizeof(int);

		//Loop through and do the same for all the client info.
		for (int i = 0; i < clientList.size(); ++i) {
			//Shorthand for clientList[i]
			const Client& client = clientList[i];

			//Identify the client by its index
			*(int*)((void*)&buf[bufIterator]) = i;
			bufIterator += sizeof(int);

			//Is a position provided (Did the position change?)
			*(bool*)((void*)&buf[bufIterator]) = client.positionChanged;
			bufIterator += sizeof(bool);

			//If so, stick the new position in there too.
			if (client.positionChanged) {
				*(vec2*)((void*)&buf[bufIterator]) = client.positionUpdate;
				bufIterator += sizeof(vec2);
			}
		}

		//Send the packet update.
		for (Client client : clientList) {
			sendto(udpSock, buf, 512, 0, (sockaddr*)&client.address, namelen);
			printf("SEND to %s: %s\n", client.name, buf);
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