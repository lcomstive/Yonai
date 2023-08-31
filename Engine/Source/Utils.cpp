#include <algorithm>
#include <glm/glm.hpp>
#include <Yonai/Utils.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.inl>

using namespace std;
using namespace glm;

string& Yonai::ToLower(string& input)
{
	transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}
