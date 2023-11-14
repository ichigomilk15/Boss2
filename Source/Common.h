#pragma once

#include <random>

namespace Common
{
	static constexpr unsigned int SQUARE_NUM_X = 8u;
	static constexpr unsigned int SQUARE_NUM_Y = 8u;
	static const float SquareWidth = 5.0f;
	static const float SquareHeight = 5.0f;

}

class CommonClass
{
public:
	static std::default_random_engine random_engine;
private:
	static std::random_device random_seed;
};