#include "RemotePaddleBehaviour.h"

#include "Net.h"
#include "glm/glm.hpp"

RemotePaddleBehaviour::RemotePaddleBehaviour(bool server, sockaddr* addrinfo, int namelen)
{
	m_server = server;
	m_addrinfo = addrinfo;
	m_namelen = namelen;
}

RemotePaddleBehaviour::~RemotePaddleBehaviour()
{

}

void RemotePaddleBehaviour::init()
{

}

void RemotePaddleBehaviour::update()
{
	std::string message = Net::recvFromUDP(m_addrinfo, &m_namelen);
	glm::vec2 position;
	sscanf_s(message.c_str(), "%f,%f", &position.x, &position.y);
}
