//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once
#include "IBehaviour.h"

#include "glm/glm.hpp"

class PuckBehaviour : public IBehaviour {
public:
	PuckBehaviour(bool _server, int _client);
	~PuckBehaviour();

	void init() override;
	void update() override;

	void hit(glm::vec2 _pos, glm::vec2 _vel);

	const float SPEED = 100.0f;
	const float MAX_SPEED_PER_FRAME = 40.0f;
private:
	glm::vec2 m_vel;
	bool m_server;
	int m_client;
};

