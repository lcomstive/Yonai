#include <algorithm>
#include <Yonai/Utils.hpp>

using namespace std;

string& Yonai::ToLower(string& input)
{
	transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}
