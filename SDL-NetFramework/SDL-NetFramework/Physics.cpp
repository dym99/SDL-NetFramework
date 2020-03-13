//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "Physics.h"

bool Physics::CollisionPointBox(const glm::vec2& _boxPos, const glm::vec2& _boxDim, const glm::vec2& _point)
{
	return ((_point.x > _boxPos.x) && (_point.x < (_boxPos.x + _boxDim.x)) &&
			(_point.y > _boxPos.y) && (_point.y < (_boxPos.y + _boxDim.y)));
}

bool Physics::CollisionBoxBox(const glm::vec2& _aPos, const glm::vec2& _aDim, const glm::vec2& _bPos, const glm::vec2& _bDim) {
	return ( ((_aPos.x + _aDim.x) >= _bPos.x) &&
		(_aPos.x <= (_bPos.x+ _bDim.x)) &&
		((_aPos.y + _aDim.y) >= _bPos.y) &&
		(_aPos.y <= (_bPos.y + _bDim.y)) );
}

bool Physics::CollisionCircleCircle(const glm::vec2& _posA, const float& _radiusA, const glm::vec2& _posB, const float& _radiusB)
{
	return (glm::distance(_posA, _posB) <= (_radiusA + _radiusB));
}
