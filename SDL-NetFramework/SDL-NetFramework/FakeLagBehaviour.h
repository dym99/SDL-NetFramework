#pragma once

#include "IBehaviour.h"

class FakeLagBehaviour : public IBehaviour
{
public:
	FakeLagBehaviour();
	~FakeLagBehaviour();

	void init() override;
	void update() override;

	const char* const name = "FLBehaviour";
};

