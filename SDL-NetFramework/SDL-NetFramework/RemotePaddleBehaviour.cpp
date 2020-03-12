#include "RemotePaddleBehaviour.h"

#include "Net.h"
#include "glm/glm.hpp"

#include "Sprite.h"

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
	if (message == "") return;
		
	glm::vec2 position;
	sscanf_s(message.c_str(), "%f,%f", &position.x, &position.y);
	position.x = 1280 - position.x;
	position.y = 720 - position.y;

	getSprite()->setPosition(position);
}
