//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include "IBehaviour.h"
#include "Net.h"

#include <unordered_map>
#include <glm/glm.hpp>

class RemotePlayerBehaviour : public IBehaviour
{
public:
	RemotePlayerBehaviour(std::unordered_map<int, struct ClientInfo>* _activeplayers, int _idx);
	~RemotePlayerBehaviour();

	void init() override;
	void update() override;

	const char* const name = "RemotePlayerBehaviour";

	glm::vec2 getLastPos() const;
	glm::vec2 getPos() const;
private:
	glm::vec2 m_lastPos;
	glm::vec2 m_currentPos;

	std::unordered_map<int, ClientInfo> *m_activeplayers;
	int m_idx;

	glm::vec2 m_vel;
};

