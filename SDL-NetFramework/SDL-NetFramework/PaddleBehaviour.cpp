//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "PaddleBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Physics.h"

#include "Net.h"

#include "PuckBehaviour.h"

#include <sstream>

PlayerBehaviour::PlayerBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck)
{
	m_server = server;
	m_addrinfo = addrinfo;
	m_namelen = namelen;
	m_puck = puck;
	m_lastPos = glm::vec2();
	m_currentPos = glm::vec2();
	m_speed = 0;
}

PlayerBehaviour::~PlayerBehaviour()
{
}

void PlayerBehaviour::init()
{
	m_currentPos = { 640, 360 };
}

void PlayerBehaviour::update()
{
	m_lastPos = m_currentPos;
	
	if (EVENTS->isKeyDown(SDLK_UP) || EVENTS->isKeyDown(SDLK_w))
	{
		//current position + y
		m_currentPos.y = m_lastPos.y - m_speed * Time::deltaTime;
	}
	
	if (EVENTS->isKeyDown(SDLK_DOWN) || EVENTS->isKeyDown(SDLK_s))
	{
		//current position - y
		m_currentPos.y = m_lastPos.y + m_speed * Time::deltaTime;
	}
	if (EVENTS->isKeyDown(SDLK_LEFT) || EVENTS->isKeyDown(SDLK_a))
	{
		//current position -x
		m_currentPos.x = m_lastPos.x - m_speed * Time::deltaTime;
	}
	if (EVENTS->isKeyDown(SDLK_RIGHT) || EVENTS->isKeyDown(SDLK_d))
	{
		//current position + x
		m_currentPos.x = m_lastPos.x + m_speed * Time::deltaTime;

	}

}
