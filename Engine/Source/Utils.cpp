#include <algorithm>
#include <Yonai/Utils.hpp>
#include <glm/gtx/matrix_decompose.inl>

using namespace std;
using namespace glm;

string& Yonai::ToLower(string& input)
{
	transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}

// Modified from <glm/gtx/matrix_decompose.inl>
bool Yonai::Decompose(mat4& matrix, vec3& outTranslation, vec3& outEulerRotation, vec3& outScale)
{
	mat4 LocalMatrix(matrix);

	// Normalize the matrix.
	if (epsilonEqual(LocalMatrix[3][3], 0.0f, epsilon<float>()))
		return false;

	// First, isolate perspective.  This is the messiest.
	if (
		epsilonNotEqual(LocalMatrix[0][3], 0.0f, epsilon<float>()) ||
		epsilonNotEqual(LocalMatrix[1][3], 0.0f, epsilon<float>()) ||
		epsilonNotEqual(LocalMatrix[2][3], 0.0f, epsilon<float>()))
	{
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = 0.0f;
		LocalMatrix[3][3] = 1.0f;
	}

	// Next take care of translation (easy).
	outTranslation = vec3(LocalMatrix[3]);
	LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

	vec3 Row[3], Pdum3;

	// Now get scale and shear.
	for (length_t i = 0; i < 3; ++i)
		for (length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	outScale.x = length(Row[0]);
	Row[0] = detail::scale(Row[0], 1.0f);
	outScale.y = length(Row[1]);
	Row[1] = detail::scale(Row[1], 1.0f);
	outScale.z = length(Row[2]);
	Row[2] = detail::scale(Row[2], 1.0f);

#if 0
	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (dot(Row[0], Pdum3) < 0)
	{
		for (length_t i = 0; i < 3; i++)
		{
			Scale[i] *= static_cast<T>(-1);
			Row[i] *= static_cast<T>(-1);
		}
	}
#endif

	outEulerRotation.y = asin(-Row[0][2]);
	if (cos(outEulerRotation.y) != 0)
	{
		outEulerRotation.x = atan2(Row[1][2], Row[2][2]);
		outEulerRotation.z = atan2(Row[0][1], Row[0][0]);
	}
	else
	{
		outEulerRotation.x = atan2(-Row[2][0], Row[1][1]);
		outEulerRotation.z = 0;
	}

	return true;
}