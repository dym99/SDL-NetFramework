//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "PlayerBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Physics.h"

#include "Net.h"

#include <sstream>

PlayerBehaviour::PlayerBehaviour()
{
	m_lastPos = glm::vec2();
	m_currentPos = glm::vec2();
	m_speed = 128;
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
	
	if (EVENTS->isKeyHeld(SDLK_UP) || EVENTS->isKeyHeld(SDLK_w))
	{
		//current position + y
		m_currentPos.y -= m_speed * Time::deltaTime;
	}
	
	if (EVENTS->isKeyHeld(SDLK_DOWN) || EVENTS->isKeyHeld(SDLK_s))
	{
		//current position - y
		m_currentPos.y += m_speed * Time::deltaTime;
	}
	if (EVENTS->isKeyHeld(SDLK_LEFT) || EVENTS->isKeyHeld(SDLK_a))
	{
		//current position -x
		m_currentPos.x -= m_speed * Time::deltaTime;
	}
	if (EVENTS->isKeyHeld(SDLK_RIGHT) || EVENTS->isKeyHeld(SDLK_d))
	{
		//current position + x
		m_currentPos.x += m_speed * Time::deltaTime;

	}

	getSprite()->setPosition(m_currentPos);
}
