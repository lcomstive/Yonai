#include <cstdlib>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Random, Seed, void, (unsigned int seed))
{ srand(seed); }

ADD_MANAGED_METHOD(Random, Int, int, (int min, int max))
{ return rand() % max + min; }

ADD_MANAGED_METHOD(Random, Float, float, (float min, float max))
{ return min + (float)rand() / ((float)RAND_MAX / (max - min)); }

