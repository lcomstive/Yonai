#include <glm/glm.hpp>
#include <gtest/gtest.h>
#include <glm/gtx/quaternion.hpp>
#include <Yonai/Components/Transform.hpp>

TEST(Transform, LocalPosition)
{
	Yonai::Components::Transform transform;

	glm::vec3 position1(1, 2, 3);
	transform.SetPosition(position1);
	EXPECT_EQ(transform.GetPosition(), position1);

	glm::vec3 position2(4, 5, 6);
	transform.SetPosition(position2);
	EXPECT_EQ(transform.GetPosition(), position2);
	EXPECT_EQ(transform.GetGlobalPosition(), position2);

	transform.SetPosition(position1 * position2);
	EXPECT_EQ(transform.GetPosition(), position1 * position2);
	EXPECT_EQ(transform.GetGlobalPosition(), position1 * position2);
}

TEST(Transform, GlobalPosition)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	EXPECT_EQ(child.GetParent(), &parent);
	EXPECT_EQ(parent.GetChildren().size(), 1);

	glm::vec3 position(2, 4, 6);
	child.SetPosition(position);
	EXPECT_EQ(child.GetPosition(), position);

	parent.SetPosition(position);
	EXPECT_EQ(parent.GetPosition(), position);
	EXPECT_EQ(child.GetPosition(), position);
	EXPECT_EQ(child.GetGlobalPosition(), position + position);

	child.SetPosition(glm::vec3(0));
	EXPECT_EQ(parent.GetPosition(), position);
	EXPECT_EQ(child.GetPosition(), glm::vec3(0));
	EXPECT_EQ(child.GetGlobalPosition(), position);

	// Parent is position	(2, 4, 6)
	// Child  is position*3 (6, 12, 18)
	// Delta  is position*2 (4, 8, 12)
	child.SetGlobalPosition(position * 3.0f);
	EXPECT_EQ(parent.GetGlobalPosition(), position);
	EXPECT_EQ(child.GetPosition(), position * 2.0f);
	EXPECT_EQ(child.GetGlobalPosition(), position * 3.0f);
}

TEST(Transform, GlobalPositionWithRotation)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	EXPECT_EQ(child.GetParent(), &parent);
	EXPECT_EQ(parent.GetChildren().size(), 1);

	glm::vec3 position(2, 4, 6);
	glm::vec3 euler(45, 0, 0);
	child.SetPosition(position);
	EXPECT_EQ(child.GetPosition(), position);

	parent.SetPosition(position);
	parent.SetRotation(euler);
	EXPECT_EQ(parent.GetPosition(), position);
	EXPECT_EQ(child.GetPosition(), position);
	EXPECT_FLOAT_EQ(parent.GetEulerRotation().x, euler.x);
	EXPECT_FLOAT_EQ(parent.GetEulerRotation().y, euler.y);
	EXPECT_FLOAT_EQ(parent.GetEulerRotation().z, euler.z);
	EXPECT_EQ(child.GetEulerRotation(), glm::vec3());
	EXPECT_FLOAT_EQ(child.GetGlobalEulerRotation().x, euler.x);
	EXPECT_FLOAT_EQ(child.GetGlobalEulerRotation().y, euler.y);
	EXPECT_FLOAT_EQ(child.GetGlobalEulerRotation().z, euler.z);

	glm::vec3 expectedGlobalPosition(4, 2.585786819458008f, 13.071067810058594f);

	EXPECT_FLOAT_EQ(child.GetGlobalPosition().x, expectedGlobalPosition.x);
	EXPECT_FLOAT_EQ(child.GetGlobalPosition().y, expectedGlobalPosition.y);
	EXPECT_FLOAT_EQ(child.GetGlobalPosition().z, expectedGlobalPosition.z);

	child.SetPosition(glm::vec3(1, 0, 0));
	EXPECT_EQ(parent.GetGlobalPosition(), position);
	EXPECT_FLOAT_EQ(child.GetPosition().x, 1.0f);
	EXPECT_FLOAT_EQ(child.GetGlobalPosition().x, position.x + 1.0f);
}

TEST(Transform, Rotation)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	glm::vec3 euler(45, 0, 0);
	glm::vec3 eulerRads = glm::radians(euler);
	glm::quat rotation(glm::radians(euler));

	EXPECT_FLOAT_EQ(eulerRads.x, glm::eulerAngles(rotation).x);
	EXPECT_FLOAT_EQ(eulerRads.y, glm::eulerAngles(rotation).y);
	EXPECT_FLOAT_EQ(eulerRads.z, glm::eulerAngles(rotation).z);

	EXPECT_FLOAT_EQ(euler.x, degrees(eulerAngles(rotation)).x);
	EXPECT_FLOAT_EQ(euler.y, degrees(eulerAngles(rotation)).y);
	EXPECT_FLOAT_EQ(euler.z, degrees(eulerAngles(rotation)).z);

	// Set quaternion
	child.SetRotation(rotation);
	EXPECT_EQ(child.GetRotation(), rotation);

	glm::vec3 childEuler = child.GetEulerRotation();
	EXPECT_FLOAT_EQ(childEuler.x, euler.x);
	EXPECT_FLOAT_EQ(childEuler.y, euler.y);
	EXPECT_FLOAT_EQ(childEuler.z, euler.z);

	// Set euler rotation
	child.SetRotation(euler);
	EXPECT_EQ(child.GetRotation(), rotation);

	childEuler = child.GetEulerRotation();
	EXPECT_FLOAT_EQ(childEuler.x, euler.x);
	EXPECT_FLOAT_EQ(childEuler.y, euler.y);
	EXPECT_FLOAT_EQ(childEuler.z, euler.z);

	// Set parent euler rotation
	parent.SetRotation(euler);
	EXPECT_EQ(parent.GetRotation(), rotation);

	childEuler = child.GetGlobalEulerRotation();
	EXPECT_FLOAT_EQ(childEuler.x, (euler + euler).x);
	EXPECT_FLOAT_EQ(childEuler.y, (euler + euler).y);
	EXPECT_FLOAT_EQ(childEuler.z, (euler + euler).z);
}

TEST(Transform, ModelMatrixScale)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	// Without modifying, model matrices should start as identity
	EXPECT_EQ(parent.GetModelMatrix(), glm::mat4(1.0f));
	EXPECT_EQ(child.GetModelMatrix(),  glm::mat4(1.0f));

	glm::vec3 globalScale(2, 3, 0.5f);
	parent.SetScale(globalScale);
	glm::mat4 modelMatrix = parent.GetModelMatrix();
	EXPECT_EQ(modelMatrix[0][0], globalScale.x);
	EXPECT_EQ(modelMatrix[1][1], globalScale.y);
	EXPECT_EQ(modelMatrix[2][2], globalScale.z);

	modelMatrix = child.GetModelMatrix();
	EXPECT_EQ(modelMatrix[0][0], globalScale.x);
	EXPECT_EQ(modelMatrix[1][1], globalScale.y);
	EXPECT_EQ(modelMatrix[2][2], globalScale.z);

	glm::vec3 localScale(2, 0.5f, 2);
	child.SetScale(localScale);
	modelMatrix = child.GetModelMatrix(); // Global
	EXPECT_EQ(modelMatrix[0][0], globalScale.x * localScale.x);
	EXPECT_EQ(modelMatrix[1][1], globalScale.y * localScale.y);
	EXPECT_EQ(modelMatrix[2][2], globalScale.z * localScale.z);

	modelMatrix = child.GetModelMatrix(false); // Local
	EXPECT_EQ(modelMatrix[0][0], localScale.x);
	EXPECT_EQ(modelMatrix[1][1], localScale.y);
	EXPECT_EQ(modelMatrix[2][2], localScale.z);
}

TEST(Transform, ModelMatrixTranslation)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	// Without modifying, model matrices should start as identity
	EXPECT_EQ(parent.GetModelMatrix(), glm::mat4(1.0f));
	EXPECT_EQ(child.GetModelMatrix(),  glm::mat4(1.0f));

	glm::vec3 globalPosition(4, 5, 2.505f);
	parent.SetPosition(globalPosition);
	glm::mat4 modelMatrix = parent.GetModelMatrix();
	EXPECT_EQ(modelMatrix[3][0], globalPosition.x);
	EXPECT_EQ(modelMatrix[3][1], globalPosition.y);
	EXPECT_EQ(modelMatrix[3][2], globalPosition.z);

	modelMatrix = child.GetModelMatrix();
	EXPECT_EQ(modelMatrix[3][0], globalPosition.x);
	EXPECT_EQ(modelMatrix[3][1], globalPosition.y);
	EXPECT_EQ(modelMatrix[3][2], globalPosition.z);

	glm::vec3 localPosition(3, -2.05f, 1.2435f);
	child.SetPosition(localPosition);
	modelMatrix = child.GetModelMatrix(); // Global
	EXPECT_EQ(modelMatrix[3][0], globalPosition.x + localPosition.x);
	EXPECT_EQ(modelMatrix[3][1], globalPosition.y + localPosition.y);
	EXPECT_EQ(modelMatrix[3][2], globalPosition.z + localPosition.z);

	modelMatrix = child.GetModelMatrix(false); // Local
	EXPECT_EQ(modelMatrix[3][0], localPosition.x);
	EXPECT_EQ(modelMatrix[3][1], localPosition.y);
	EXPECT_EQ(modelMatrix[3][2], localPosition.z);
}

TEST(Transform, ModelMatrixRotation)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	// Without modifying, model matrices should start as identity
	EXPECT_EQ(parent.GetModelMatrix(), glm::mat4(1.0f));
	EXPECT_EQ(child.GetModelMatrix(),  glm::mat4(1.0f));

	glm::quat globalRotation(glm::vec3(45, 0, -90));
	parent.SetRotation(globalRotation);

	EXPECT_EQ(parent.GetModelMatrix(), glm::toMat4(globalRotation));
	EXPECT_EQ(child.GetModelMatrix(),  glm::toMat4(globalRotation));

	glm::quat localRotation(glm::vec3(-45, 30, 16));
	child.SetRotation(localRotation);
	glm::mat4 modelMatrix = child.GetModelMatrix(); // Global
	EXPECT_EQ(modelMatrix, glm::toMat4(globalRotation) * glm::toMat4(localRotation));

	modelMatrix = child.GetModelMatrix(false); // Local
	EXPECT_EQ(modelMatrix, glm::toMat4(localRotation));
}

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
TEST(Transform, ModelMatrixDecompose)
{
	Yonai::Components::Transform parent, child;
	child.SetParent(&parent);

	parent.SetPosition({ 3, 1, 4 });
	child.SetPosition({ 2, 7, 3 });
	parent.UpdateModelMatrices();

	EXPECT_EQ(child.GetGlobalPosition(),
		parent.GetPosition() + child.GetPosition());

	glm::vec3 position, euler, scale;
	ImGuizmo::DecomposeMatrixToComponents(
		glm::value_ptr(child.GetModelMatrix()),
		glm::value_ptr(position),
		glm::value_ptr(euler),
		glm::value_ptr(scale)
	);

	EXPECT_EQ(position, child.GetGlobalPosition());
	EXPECT_EQ(euler, child.GetGlobalEulerRotation());
	EXPECT_EQ(scale, child.GetGlobalScale());
}