
#include "Common.h"

std::random_device CommonClass::random_seed;
std::default_random_engine CommonClass::random_engine = std::default_random_engine(CommonClass::random_seed());
std::mt19937 CommonClass::random = std::mt19937(CommonClass::random_engine);

const int CommonClass::GetDirectionTarget(const DirectX::XMINT2 initPos, const DirectX::XMINT2& targetPos)
{
	const DirectX::XMINT2 dirPos = { targetPos.x - initPos.x, targetPos.y - initPos.y };
	if (dirPos.y > 0)
	{
		if (dirPos.x > 0)
			return DirectionFace::BackRight;
		else if (dirPos.x < 0)
			return DirectionFace::BackLeft;
		else
			return DirectionFace::Back;
	}
	else if (dirPos.y < 0)
	{
		if (dirPos.x > 0)
			return DirectionFace::FrontRight;
		else if (dirPos.x < 0)
			return DirectionFace::FrontLeft;
		else
			return DirectionFace::Front;
	}
	else if (dirPos.x > 0)
	{
		return DirectionFace::Right;
	}
	else
	{
		return DirectionFace::Left;
	}
}
