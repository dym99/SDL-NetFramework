//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "RemotePlayerBehaviour.h"

#include "ClientInfo.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Physics.h"

#include "Net.h"

#include <sstream>

RemotePlayerBehaviour::RemotePlayerBehaviour(std::unordered_map<int, struct ClientInfo>* _activeplayers, int _idx)
{
	m_lastPos = glm::vec2();
	m_currentPos = glm::vec2();
	m_activeplayers = _activeplayers;
	m_idx = _idx;
	m_vel = glm::vec2();
	printf("IDX: %d\n", _idx);
}

RemotePlayerBehaviour::~RemotePlayerBehaviour()
{
}

void RemotePlayerBehaviour::init()
{
	m_currentPos = { 640, 360 };
}

void RemotePlayerBehaviour::update()
{
	//Update position
	
	//If the position has been updated...
	if (m_activeplayers->at(m_idx).posChanged) {
		//Set it.
		m_lastPos = m_currentPos;
		m_currentPos = m_activeplayers->at(m_idx).pos;

		m_vel = (m_currentPos - m_lastPos)/Time::deltaTime;
		
	}
	else {
		//Interpret the next position based on previous velocity.
		m_lastPos = m_currentPos;
		m_currentPos = m_currentPos + m_vel * Time::deltaTime;
	}

	getSprite()->setPosition(m_currentPos);
}

glm::vec2 RemotePlayerBehaviour::getLastPos() const
{
	return m_lastPos;
}

glm::vec2 RemotePlayerBehaviour::getPos() const
{
	return m_currentPos;
}
