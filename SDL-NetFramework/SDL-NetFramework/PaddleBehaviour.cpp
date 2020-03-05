#include "PaddleBehaviour.h"

#include "Sprite.h"
#include "Events.h"
#include "Time.h"

#include "Debug.h"

#include "Net.h"

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
	getSprite()->setPosition((position-getSprite()->getDimensions()*0.5f));
	
	
}
