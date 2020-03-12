#pragma once

#include "IBehaviour.h"
#include "Net.h"

class PaddleBehaviour : public IBehaviour
{
public:
	PaddleBehaviour(bool server, sockaddr* addrinfo, int namelen, Sprite* puck);
	~PaddleBehaviour();

	void init() override;
	void update() override;

private:
	bool m_server;
	sockaddr* m_addrinfo;
	int m_namelen;
	Sprite* m_puck;
};

