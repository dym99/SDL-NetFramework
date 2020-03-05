#include "IBehaviour.h"

void IBehaviour::setSprite(Sprite* _sprite)
{
	m_sprite = _sprite;
}

Sprite* IBehaviour::getSprite() const
{
	return m_sprite;
}
