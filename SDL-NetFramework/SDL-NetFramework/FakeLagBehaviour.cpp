#include "FakeLagBehaviour.h"


#include "Events.h"
#include "Net.h"
#pragma comment(lib, "User32.lib")

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
	if (EVENTS->isKeyDown(SDLK_EQUALS)) {
		Net::incLag();
		printf("Simulated Lag: %i milliseconds.\n", Net::getLag());
	}
	if (EVENTS->isKeyDown(SDLK_MINUS)) {
		Net::decLag();
		printf("Simulated Lag: %i milliseconds.\n", Net::getLag());
	}
	if (GetAsyncKeyState(VK_UP)) {
	}
	if (GetAsyncKeyState(VK_DOWN)) {
	}
}
