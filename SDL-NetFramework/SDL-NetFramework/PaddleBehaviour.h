#pragma once

#include "IBehaviour.h"

class PaddleBehaviour : public IBehaviour
{
public:
	PaddleBehaviour(float _speed);
	~PaddleBehaviour();

	void init() override;
	void update() override;

private:
	float m_speed;
};

