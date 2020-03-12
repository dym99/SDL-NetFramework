#include "RemotePaddleBehaviour.h"

#include "Net.h"
#include "glm/glm.hpp"

#include "PuckBehaviour.h"

#include "Sprite.h"

RemotePaddleBehaviour::RemotePaddleBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck)
{
	m_server = server;
	m_addrinfo = addrinfo;
	m_namelen = namelen;
	m_puck = puck;
}

RemotePaddleBehaviour::~RemotePaddleBehaviour()
{

}

void RemotePaddleBehaviour::init()
{

}

void RemotePaddleBehaviour::update()
{
	std::string message = "";
	while (1) {
		message = Net::recvFromUDP(m_addrinfo, &m_namelen);
		if (message == "") return;

		if (
			message[0] == '[' &&
			message[1] == 'p' &&
			message[2] == 'u' &&
			message[3] == 'c' &&
			message[4] == 'k' &&
			message[5] == ']') {
			//Puck message
			glm::vec2 position = {};
			glm::vec2 velocity = {};

			sscanf_s(message.c_str(), "[puck]%f,%f,%f,%f", &position.x, &position.y, &velocity.x, &velocity.y);

			position.x = 1280 - position.x;
			position.y = 720 - position.y;
			velocity.x *= -1;
			velocity.y *= -1;

			m_puck->getBehaviour<PuckBehaviour>()->hit(position, velocity);
		}
		else {
			//Paddle message
			glm::vec2 position = {};
			sscanf_s(message.c_str(), "[paddle]%f,%f", &position.x, &position.y);
			position += getSprite()->getDimensions() * 0.5f;

			position.x = 1280 - position.x;
			position.y = 720 - position.y;

			getSprite()->setPosition(position);
		}

	}
}
