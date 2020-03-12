#include "PaddleBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Physics.h"

#include "Net.h"

#include "PuckBehaviour.h"

#include <sstream>

PaddleBehaviour::PaddleBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck)
{
	m_server = server;
	m_addrinfo = addrinfo;
	m_namelen = namelen;
	m_puck = puck;
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
		message << "[paddle]" << position.x << "," << position.y;
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

	glm::vec2 puckpos = m_puck->getPosition() + m_puck->getDimensions() * 0.5f;
	if (Physics::CollisionCircleCircle(position, 24.0f, puckpos, 16.0f)) {
		//Bounce
		glm::vec2 dir = glm::vec2();
		dir = puckpos - position;

		std::stringstream puckMessage;
		puckMessage << "[puck]" << puckpos.x << "," << puckpos.y << "," << dir.x << "," << dir.y;
		if (m_server) {
			Net::sendToUDP(m_addrinfo, m_namelen, puckMessage.str());
		}
		else {
			Net::sendToUDP(m_addrinfo, m_namelen, puckMessage.str());
		}

		m_puck->getBehaviour<PuckBehaviour>()->hit(puckpos, dir);
	}
	
}
