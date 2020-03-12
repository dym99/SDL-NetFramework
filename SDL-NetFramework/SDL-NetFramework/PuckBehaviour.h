#pragma once
#include "IBehaviour.h"

#include "glm/glm.hpp"

class PuckBehaviour : public IBehaviour {
public:
	PuckBehaviour(bool _server);
	~PuckBehaviour();

	void init() override;
	void update() override;

	void hit(glm::vec2 _pos, glm::vec2 _vel);

	const float SPEED = 100.0f;
	const float MAX_SPEED_PER_FRAME = 40.0f;
private:
	glm::vec2 m_vel;
	bool m_server;
};

