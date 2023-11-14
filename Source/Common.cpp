
#include "Common.h"

std::random_device CommonClass::random_seed;
std::default_random_engine CommonClass::random_engine = std::default_random_engine(CommonClass::random_seed());