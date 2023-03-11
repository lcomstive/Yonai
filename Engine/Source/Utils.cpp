#include <algorithm>
#include <AquaEngine/Utils.hpp>

using namespace std;

string& AquaEngine::ToLower(string& input)
{
	transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}
