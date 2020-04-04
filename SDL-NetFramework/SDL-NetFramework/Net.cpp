//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "Net.h"

#include "Debug.h"

SOCKET Net::m_TCPSock = INVALID_SOCKET;
SOCKET Net::m_UDPSock = INVALID_SOCKET;

int Net::m_lagMilliseconds = 0;

bool Net::m_TCPOpen = false;
bool Net::m_UDPOpen = false;

#include <thread>

void Net::init()
{
	WSADATA data;
	int error = WSAStartup(MAKEWORD(2, 2), &data);
	if (error!=0) {
		DEBUG_LOG("Error initialising WinSock: %i\n", error);
	}
}

void Net::cleanup()
{
	WSACleanup();
}

bool Net::startUDPSocket()
{
	m_UDPSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_UDPSock == INVALID_SOCKET) {
		DEBUG_LOG("Failed to open UDP socket! WSAError: %ld", WSAGetLastError());
		m_UDPOpen = false;
		return false;
	}
	m_UDPOpen = true;

	unsigned long iMode = 1;
	int iResult = ioctlsocket(m_UDPSock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);



	return true;
}

bool Net::startTCPSocket()
{
	m_TCPSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_TCPSock == INVALID_SOCKET) {
		DEBUG_LOG("Failed to open TCP socket! WSAError: %ld\n", WSAGetLastError());
		m_TCPOpen = false;
		return false;
	}
	m_TCPOpen = true;
	return true;
}

SOCKET Net::getUDPSocket()
{
	return m_UDPSock;
}

SOCKET Net::getTCPSocket()
{
	return m_TCPSock;
}

bool Net::bindUDP(unsigned short port)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		return false;
	}
	sockaddr_in addrinfo = {};
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.s_addr = htonl(0);
	addrinfo.sin_port = htons(port);
	
	if (bind(m_UDPSock, (const sockaddr*)&addrinfo, sizeof(sockaddr_in))==SOCKET_ERROR) {
		DEBUG_LOG("UDP bind failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool Net::bindTCP(unsigned short port)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	sockaddr_in addrinfo = {};
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.s_addr = htonl(0);
	addrinfo.sin_port = htons(port);

	if (bind(m_TCPSock, (const sockaddr*)&addrinfo, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		DEBUG_LOG("TCP bind failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool Net::listenTCP()
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	if (listen(m_TCPSock, NET_MAX_CONNECTIONS) == SOCKET_ERROR) {
		DEBUG_LOG("TCP listen failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool Net::connectTCP(const sockaddr* addrinfo, int namelen)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	if (connect(m_TCPSock, addrinfo, namelen) == SOCKET_ERROR) {
		DEBUG_LOG("Connection failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}

	//Non-blocking mode after connection
	unsigned long iMode = 1;
	int iResult = ioctlsocket(m_TCPSock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	return true;
}

int Net::acceptTCP(sockaddr* addr, int* namelen)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}

	int res = accept(m_TCPSock, addr, namelen);

	return res;
}

bool Net::sendTCP(std::string data)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	int lagms = m_lagMilliseconds;
	//Execute send on a separate thread after a delay.
	std::thread([&, lagms, data] {
		std::this_thread::sleep_for(std::chrono::milliseconds(lagms));
		send(m_TCPSock, data.c_str(), data.size(), NULL);
		}).detach();
	//if (send(m_TCPSock, data.c_str(), data.size(), NULL) == SOCKET_ERROR) {
	//	DEBUG_LOG("TCP Send failed! WSAError: %ld\n", WSAGetLastError());
	//	return false;
	//}
	//else {
	//	DEBUG_LOG("[TCP] Send: %s\n", data.c_str());
	//}
	return true;
}

bool Net::sendTCP(int destination, std::string data)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	int lagms = m_lagMilliseconds;
	//Execute send on a separate thread after a delay.
	std::thread([&, lagms, data, destination] {
		std::this_thread::sleep_for(std::chrono::milliseconds(lagms));
		send(destination, data.c_str(), min(data.size(), NET_BUFFER_LEN), NULL);
		}).detach();
	//if (send(destination, data.c_str(), min(data.size(), NET_BUFFER_LEN), NULL) == SOCKET_ERROR) {
	//	DEBUG_LOG("TCP Send failed! WSAError: %ld\n", WSAGetLastError());
	//	return false;
	//}
	//else {
	//	DEBUG_LOG("[TCP] Send: %s\n", data.c_str());
	//}
	return true;
}

std::string Net::recvTCP()
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return "";
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recv(m_TCPSock, buf, NET_BUFFER_LEN, NULL);

	//Null terminate, just in case.
	buf[NET_BUFFER_LEN - 1] = '\0';

	if (bytes == 0) {
		DEBUG_LOG("TCP recv failed! WSAError: %ld", WSAGetLastError());
	}
	else {
		DEBUG_LOG("[TCP] Recv: %s\n", buf);
	}

	return std::string(buf);
}

std::string Net::recvTCP(int from)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return "";
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recv(from, buf, NET_BUFFER_LEN, NULL);
	
	if (bytes == 0) {
		DEBUG_LOG("TCP recv failed! WSAError: %ld", WSAGetLastError());
	}
	else {
		DEBUG_LOG("[TCP] Recv: %s\n", buf);
	}

	//Null-terminate, just in case.
	buf[NET_BUFFER_LEN - 1] = '\0';
	return std::string(buf);
}

bool Net::sendToUDP(const sockaddr* destination, int namelen, std::string data)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		return false;
	}
	int lagms = m_lagMilliseconds;
	int udpsock = m_UDPSock;
	//Execute send on a separate thread after a delay.
	std::thread([&, lagms, data, udpsock, destination, namelen] {
		std::this_thread::sleep_for(std::chrono::milliseconds(lagms));
		sendto(udpsock, data.data(), min(data.size(), NET_BUFFER_LEN), NULL, destination, namelen);
	}).detach();
	return true;
}

std::string Net::recvFromUDP(sockaddr* from, int* fromlen)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		return "";
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recvfrom(m_UDPSock, buf, NET_BUFFER_LEN, NULL, from, fromlen);

	//Null-terminate, just in case.
	buf[NET_BUFFER_LEN - 1] = '\0';

	if (bytes == 0) {
		DEBUG_LOG("UDP recv failed! WSAError: %ld\n", WSAGetLastError());
	}
	else {
		//DEBUG_LOG("[UDP] Recv: %s\n", buf);
	}

	return std::string(buf);
}

bool Net::sendToUDPBin(const sockaddr* destination, int namelen, void* data, int datalen)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		return false;
	}
	int lagms = m_lagMilliseconds;
	int udpsock = m_UDPSock;
	//Execute send on a separate thread after a delay.
	std::thread([&, lagms, data, datalen, udpsock, destination, namelen] {
		std::this_thread::sleep_for(std::chrono::milliseconds(lagms));
		sendto(udpsock, (const char*)data, datalen, NULL, destination, namelen);
		}).detach();
}

void* Net::recvFromUDPBin(sockaddr* from, int* fromlen, int& recvlen)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		recvlen = 0;
		return NULL;
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recvfrom(m_UDPSock, buf, NET_BUFFER_LEN, NULL, from, fromlen);

	//Return the data and pass the size.
	recvlen = bytes;
	return (void*)&buf[0];
}

void Net::closeUDPSocket()
{
	closesocket(m_UDPSock);
}

void Net::closeTCPSocket()
{
	closesocket(m_TCPSock);
}

void Net::incLag()
{
	m_lagMilliseconds += 10;
}

void Net::decLag()
{
	m_lagMilliseconds -= 10;
	if (m_lagMilliseconds < 0) m_lagMilliseconds = 0;
}
