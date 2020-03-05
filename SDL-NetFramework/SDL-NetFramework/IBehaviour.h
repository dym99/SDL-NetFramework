#pragma once

class Sprite;

class IBehaviour abstract
{
public:
	virtual void init() = 0;
	virtual void update() = 0;

	void setSprite(Sprite* _sprite);
	Sprite* getSprite() const;
private:
	Sprite* m_sprite;
};

