//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

#define NET_MAX_CONNECTIONS 2
#define NET_BUFFER_LEN 512

class Net
{
public:
	static void init();
	static void cleanup();

	//Socket creation
	static bool startUDPSocket();
	static bool startTCPSocket();

	//Socket getters
	static SOCKET getUDPSocket();
	static SOCKET getTCPSocket();

	//Socket binding
	static bool bindUDP(unsigned short port);
	static bool bindTCP(unsigned short port);

	//TCP proto stuff
	static bool listenTCP();
	static bool connectTCP(const sockaddr* addrinfo, int namelen);
	static int acceptTCP(sockaddr* addr, int* namelen);
	static bool sendTCP(std::string data);
	static bool sendTCP(int destination, std::string data);
	static std::string recvTCP();
	static std::string recvTCP(int from);

	//UDP proto stuff
	static bool sendToUDP(const sockaddr* destination, int namelen, std::string data);
	static std::string recvFromUDP(sockaddr* from, int* fromlen);

	//Close sockets
	static void closeUDPSocket();
	static void closeTCPSocket();

	//Simulated lag
	static void incLag();
	static void decLag();
	inline static int getLag() { return m_lagMilliseconds; }
private:
	static SOCKET m_UDPSock;
	static SOCKET m_TCPSock;

	static bool m_UDPOpen;
	static bool m_TCPOpen;

	static int m_lagMilliseconds;
};

