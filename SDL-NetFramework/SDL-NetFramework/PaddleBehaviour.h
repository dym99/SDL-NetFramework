//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include "IBehaviour.h"
#include "Net.h"

#include <glm/glm.hpp>

class PaddleBehaviour : public IBehaviour
{
public:
	PaddleBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck);
	~PaddleBehaviour();

	void init() override;
	void update() override;

	const char* const name = "PaddleBehaviour";
private:
	bool m_server;
	sockaddr* m_addrinfo;
	int m_namelen;
	Sprite* m_puck;
	glm::vec2 m_lastPos;
	float m_speed;
};

