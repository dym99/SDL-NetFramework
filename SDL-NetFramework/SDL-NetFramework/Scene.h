#pragma once

#include "Sprite.h"
#include <vector>

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void init();

	virtual void update();

	virtual void render();

	void addSprite(Sprite* _sprite);
private:
	std::vector<Sprite*> m_sprites;
};

