//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include "IBehaviour.h"
#include "Net.h"

#include <glm/glm.hpp>

class PlayerBehaviour : public IBehaviour
{
public:
	PlayerBehaviour();
	~PlayerBehaviour();

	void init() override;
	void update() override;

	const char* const name = "PlayerBehaviour";
private:
	glm::vec2 m_lastPos;
	glm::vec2 m_currentPos;
	float m_speed;
};

