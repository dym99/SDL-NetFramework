#include "PaddleBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Net.h"

#include <sstream>

PaddleBehaviour::PaddleBehaviour(bool server, sockaddr* addrinfo, int namelen)
{
	m_server = server;
	m_addrinfo = addrinfo;
	m_namelen = namelen;
}

PaddleBehaviour::~PaddleBehaviour()
{
}

void PaddleBehaviour::init()
{

}

void PaddleBehaviour::update()
{
	//float direction = 0.0f;
	//if (EVENTS->isKeyHeld(SDLK_UP)) {
	//	direction -= 1.0f;
	//}
	//if (EVENTS->isKeyHeld(SDLK_DOWN)) {
	//	direction += 1.0f;
	//}
	//
	//glm::vec2 position = getSprite()->getPosition();
	//position.y += (direction * m_speed * Time::deltaTime);

	int x, y;
	EVENTS->getMousePosition(&x, &y);
	glm::vec2 position = { (float)x, (float)y };
	

		std::stringstream message;
		message << position.x << "," << position.y;
		if (m_server) {
			Net::sendToUDP(m_addrinfo, m_namelen, message.str());
		}
		else {
			Net::sendToUDP(m_addrinfo, m_namelen, message.str());
		}
		
		//std::string message = Net::recvFromUDP(m_addrinfo, &m_namelen);
		//glm::vec2 position;
		//sscanf(message.c_str(), "%f,%f", &position.x, &position.y);

	getSprite()->setPosition((position - getSprite()->getDimensions() * 0.5f));
	
}
