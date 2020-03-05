#pragma once

#include <glm/glm.hpp>

/*

Basic static physics functions contained in an abstract class

*/
class Physics abstract
{
public:
	static bool CollisionPointBox(const glm::vec2& _boxPos, const glm::vec2& _boxDim, const glm::vec2& _point);
	static bool CollisionBoxBox(const glm::vec2& _aPos, const glm::vec2& _aDim, const glm::vec2& _bPos, const glm::vec2& _bDim);
};

