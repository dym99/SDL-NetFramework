#pragma once
#include "IBehaviour.h"

#include "glm/glm.hpp"

class PuckBehaviour : public IBehaviour {
public:
	PuckBehaviour();
	~PuckBehaviour();

	void init() override;
	void update() override;

	void hit(glm::vec2 _pos, glm::vec2 _dir);

	const float SPEED = 100.0f;
private:
	glm::vec2 m_vel;
};

