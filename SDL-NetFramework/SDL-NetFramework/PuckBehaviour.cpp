#include "PuckBehaviour.h"

#include "Sprite.h"
#include "Time.h"

PuckBehaviour::PuckBehaviour()
{
	m_vel = glm::vec2();
}

PuckBehaviour::~PuckBehaviour()
{
}

void PuckBehaviour::init()
{
}

void PuckBehaviour::update()
{
	glm::vec2 pos = getSprite()->getPosition();

	//physics
	pos += m_vel * Time::deltaTime;

	//Bounce off of sides
	if (pos.x < 0 || (pos.x + getSprite()->getDimensions().x) > 1280) {
		pos.x *= -1;
		m_vel.x *= -1;
	}
	if (pos.y < 0 || (pos.y + getSprite()->getDimensions().y) > 720) {
		pos.y *= -1;
		m_vel.y *= -1;
	}

	//Update position
	getSprite()->setPosition(pos);
}

void PuckBehaviour::hit(glm::vec2 _pos, glm::vec2 _dir)
{
	getSprite()->setPosition(_pos - (getSprite()->getDimensions()*0.5f));
	m_vel = glm::normalize(_dir) * SPEED;
}
