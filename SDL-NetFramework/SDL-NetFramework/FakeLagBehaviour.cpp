#include "FakeLagBehaviour.h"

#include "Events.h"
#include "Net.h"

FakeLagBehaviour::FakeLagBehaviour()
{
}

FakeLagBehaviour::~FakeLagBehaviour()
{
}

void FakeLagBehaviour::init()
{
}

void FakeLagBehaviour::update()
{
	if (EVENTS->isKeyDown(SDLK_PLUS)) {
		Net::incLag();
	}
	if (EVENTS->isKeyDown(SDLK_MINUS)) {
		Net::decLag();
	}
}
