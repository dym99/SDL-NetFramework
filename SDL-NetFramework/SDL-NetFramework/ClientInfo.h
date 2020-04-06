#pragma once
#include "Sprite.h"
#include "glm/glm.hpp"
#include "RemotePlayerBehaviour.h"
struct ClientInfo {
	bool posChanged = false;
	glm::vec2 pos = {};
	Sprite* remotePlayerSprite = nullptr;
	RemotePlayerBehaviour* remotePlayer = nullptr;
	bool hasAvatar = 0;
};