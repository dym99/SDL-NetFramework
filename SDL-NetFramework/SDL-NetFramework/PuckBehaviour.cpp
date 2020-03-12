#include "PuckBehaviour.h"

#include "Sprite.h"
#include "Time.h"

PuckBehaviour::PuckBehaviour(bool _server)
{
	m_vel = glm::vec2();
	m_server = _server;
}

PuckBehaviour::~PuckBehaviour()
{
}

void PuckBehaviour::init()
{
	glm::vec2 pos = getSprite()->getPosition()+(getSprite()->getDimensions()*0.5f);
	//Flip for the client
	if (!m_server) {
		pos.x = 1280 - pos.x;
		pos.y = 720 - pos.y;
	}
}

void PuckBehaviour::update()
{
	glm::vec2 pos = getSprite()->getPosition();

	//physics
	pos += m_vel * Time::deltaTime;

	//Bounce off of sides
	if (pos.x < 0 || (pos.x + getSprite()->getDimensions().x) > 1280.0f) {
		if (pos.x < 0)
			pos.x *= -1;
		else
			pos.y -= pos.y - 1280.0f;
		m_vel.x *= -1;
	}
	if (pos.y < 0 || (pos.y + getSprite()->getDimensions().y) > 720.0f) {
		if (pos.y < 0)
			pos.y *= -1;
		else
			pos.y -= pos.y - 720.0f;
		m_vel.y *= -1;
	}

	//Update position
	getSprite()->setPosition(pos);
}

void PuckBehaviour::hit(glm::vec2 _pos, glm::vec2 _dir)
{
	glm::vec2 pos = _pos;
	//Flip for the client
	if (!m_server) {
		pos.x = 1280 - pos.x;
		pos.y = 720 - pos.y;

		_dir.x *= -1;
		_dir.y *= -1;
	}
	getSprite()->setPosition(pos - (getSprite()->getDimensions()*0.5f));
	m_vel = glm::normalize(_dir) * SPEED;
}
