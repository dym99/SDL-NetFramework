//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include "IBehaviour.h"
#include "Net.h"

#include <glm/glm.hpp>

class PlayerBehaviour : public IBehaviour
{
public:
	PlayerBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck);
	~PlayerBehaviour();

	void init() override;
	void update() override;

	const char* const name = "PaddleBehaviour";
private:
	bool m_server;
	sockaddr* m_addrinfo;
	int m_namelen;
	Sprite* m_puck;
	glm::vec2 m_lastPos;
	glm::vec2 m_currentPos;
	float m_speed;
};

