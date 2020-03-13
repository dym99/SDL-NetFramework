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
	//if (EVENTS->isKeyDown(SDLK_p)) {
	if (GetAsyncKeyState(VK_UP)) {
		Net::incLag();
	}
	//if (EVENTS->isKeyDown(SDLK_o)) {
	if (GetAsyncKeyState(VK_DOWN)) {
		Net::decLag();
		printf("YEE");
	}
}
