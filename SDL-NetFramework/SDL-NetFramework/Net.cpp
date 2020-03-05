#include "Net.h"

#include "Debug.h"

SOCKET Net::m_TCPSock = INVALID_SOCKET;
SOCKET Net::m_UDPSock = INVALID_SOCKET;


bool Net::m_TCPOpen = false;
bool Net::m_UDPOpen = false;


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
	return true;
}

int Net::acceptTCP()
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	return accept(m_TCPSock, NULL, NULL);
}

bool Net::sendTCP(std::string data)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	if (send(m_TCPSock, data.c_str(), data.size(), NULL) == SOCKET_ERROR) {
		DEBUG_LOG("TCP Send failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool Net::sendTCP(int destination, std::string data)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	if (send(destination, data.c_str(), min(data.size(), NET_BUFFER_LEN), NULL) == SOCKET_ERROR) {
		DEBUG_LOG("TCP Send failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}
	return true;
}

std::string Net::recvTCP()
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recv(m_TCPSock, buf, NET_BUFFER_LEN, NULL);

	if (bytes == 0) {
		DEBUG_LOG("TCP recv failed! WSAError: %ld", WSAGetLastError());
	}

	//Null terminate, just in case.
	buf[NET_BUFFER_LEN - 1] = '\0';
	return std::string(buf);
}

std::string Net::recvTCP(int from)
{
	if (!m_TCPOpen) {
		DEBUG_LOG("TCP socket is not open!\n");
		return false;
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recv(from, buf, NET_BUFFER_LEN, NULL);
	
	if (bytes == 0) {
		DEBUG_LOG("TCP recv failed! WSAError: %ld", WSAGetLastError());
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
	if (sendto(m_UDPSock, data.data(), min(data.size(), NET_BUFFER_LEN), NULL, destination, namelen) == SOCKET_ERROR) {
		DEBUG_LOG("UDP sendto failed! WSAError: %ld\n", WSAGetLastError());
		return false;
	}
	return true;
}

std::string Net::recvFromUDP(sockaddr* from, int* fromlen)
{
	if (!m_UDPOpen) {
		DEBUG_LOG("UDP socket is not open!\n");
		return false;
	}
	//Create empty buffer
	char buf[NET_BUFFER_LEN];
	ZeroMemory(buf, NET_BUFFER_LEN);

	//Recieve
	int bytes = recvfrom(m_UDPSock, buf, NET_BUFFER_LEN, NULL, from, fromlen);

	if (bytes == 0) {
		DEBUG_LOG("UDP recv failed! WSAError: %ld", WSAGetLastError());
	}

	//Null-terminate, just in case.
	buf[NET_BUFFER_LEN - 1] = '\0';
	return std::string(buf);
}

void Net::closeUDPSocket()
{
	closesocket(m_UDPSock);
}

void Net::closeTCPSocket()
{
	closesocket(m_TCPSock);
}