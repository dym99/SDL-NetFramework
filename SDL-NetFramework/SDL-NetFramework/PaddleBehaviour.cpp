#include "PaddleBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

PaddleBehaviour::PaddleBehaviour(float _speed)
{
	m_speed = _speed;
}

PaddleBehaviour::~PaddleBehaviour()
{
}

void PaddleBehaviour::init()
{

}

void PaddleBehaviour::update()
{
	float direction = 0.0f;
	if (EVENTS->isKeyHeld(SDLK_UP)) {
		direction -= 1.0f;
	}
	if (EVENTS->isKeyHeld(SDLK_DOWN)) {
		direction += 1.0f;
	}

	glm::vec2 position = getSprite()->getPosition();
	position.y += (direction * m_speed * Time::deltaTime);
	getSprite()->setPosition(position);
}
