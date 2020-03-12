#pragma once

#include "IBehaviour.h"
#include "Net.h"

#include <glm/glm.hpp>

class RemotePaddleBehaviour : public IBehaviour
{
public:
	RemotePaddleBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck);
	~RemotePaddleBehaviour();

	void init() override;
	void update() override;
private:
	bool m_server;
	sockaddr* m_addrinfo;
	int m_namelen;
	Sprite* m_puck;
	glm::vec2 m_lastPos;
};
