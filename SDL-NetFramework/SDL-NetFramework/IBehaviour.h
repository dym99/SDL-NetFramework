//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

class Sprite;

class IBehaviour abstract
{
public:
	virtual void init() = 0;
	virtual void update() = 0;

	void setSprite(Sprite* _sprite);
	Sprite* getSprite() const;
	const char* const name = "IBehaviour";
private:
	Sprite* m_sprite;
};

