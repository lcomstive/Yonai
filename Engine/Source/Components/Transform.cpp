#include <Yonai/World.hpp>
#include <Yonai/Utils.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <Yonai/Components/Transform.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics;
using namespace Yonai::Components;

vec3 Transform::GetPosition() { return Position; }
quat Transform::GetRotation() { return Rotation; }
vec3 Transform::GetScale() { return Scale; }
vec3 Transform::GetEulerRotation() { return degrees(eulerAngles(Rotation)); }

vec3 Transform::GetGlobalPosition()
// { return Position + (m_Parent ? m_Parent->GetGlobalPosition() : vec3(0)); }
{ return (m_Parent ? m_Parent->GetModelMatrix() : mat4(1.0f)) * vec4(Position, 1); }

quat Transform::GetGlobalRotation()
{
	if (m_Parent)
		return m_Parent->GetGlobalRotation() * Rotation;
	else
		return Rotation;
}

vec3 Transform::GetGlobalEulerRotation() { return degrees(eulerAngles(GetGlobalRotation())); }

vec3 Transform::GetGlobalScale()
{ return Scale * (m_Parent ? m_Parent->GetGlobalScale() : vec3(1.0f)); }

vec3 Transform::Up()	  { return vec3(0, 1, 0) * Rotation; }
vec3 Transform::Right()	  { return vec3(1, 0, 0) * Rotation; }
vec3 Transform::Forward() { return vec3(0, 0, 1) * Rotation; }

Transform* Transform::GetParent() { return m_Parent; }
void Transform::SetParent(Transform* parent)
{
	if (m_Parent) m_Parent->RemoveChild(this);

	m_Parent = parent;

	if (m_Parent) m_Parent->AddChild(this);
}

void Transform::AddChild(Transform* child)
{
	UUID id = child->Entity.ID();
	if (m_Children.find(id) == m_Children.end())
		m_Children.emplace(id, child);
	child->m_Parent = this;
	child->m_IsDirty = true;
}

void Transform::RemoveChild(Transform* child)
{
	UUID id = child->Entity.ID();
	if (m_Children.find(id) != m_Children.end())
		m_Children.erase(id);
	child->m_Parent = nullptr;
	child->m_IsDirty = true;
}

vector<Transform*> Transform::GetChildren()
{
	vector<Transform*> output;
	output.reserve(m_Children.size());
	for (const auto pair : m_Children)
		output.push_back(pair.second);
	return output;
}

void Transform::SetPosition(vec3 position)
{
	m_IsDirty = true;
	Position = position;
}

void Transform::SetRotation(quat rotation)
{
	m_IsDirty = true;
	Rotation = rotation;
}

void Transform::SetRotation(vec3 euler)
{
	m_IsDirty = true;
	Rotation = quat(radians(euler));
}

void Transform::SetScale(vec3 scale)
{
	m_IsDirty = true;
	Scale = scale;
}

void Transform::SetGlobalPosition(vec3 position)
{
	if (m_Parent)
		position -= m_Parent->GetGlobalPosition();

	Position = position;
	m_IsDirty = true;
}

void Transform::SetGlobalRotation(vec3 euler)
{
	Rotation = glm::quat(glm::radians(euler));
	if (m_Parent)
		Rotation = m_Parent->GetGlobalRotation() * inverse(Rotation);
	m_IsDirty = true;
}

void Transform::SetGlobalRotation(quat rotation)
{
	Rotation = rotation;
	if (m_Parent)
		Rotation *= m_Parent->GetGlobalRotation();
	m_IsDirty = true;
}

void Transform::SetGlobalScale(vec3 scale)
{
	Scale = scale;
	if (m_Parent)
		Scale /= m_Parent->GetGlobalScale();
	m_IsDirty = true;
}

void Transform::UpdateModelMatrices(bool force)
{
	if (!m_IsDirty && !force)
		return;

	// Generate model matrix
	/*
	mat4 translationMatrix = translate(mat4(1.0f), Position);
	mat4 scaleMatrix = scale(mat4(1.0f), Scale);
	mat4 rotationMatrix = toMat4(Rotation);

	ModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

	if (m_Parent)
		GlobalModelMatrix = m_Parent->GlobalModelMatrix * ModelMatrix;
	else
		GlobalModelMatrix = ModelMatrix;
	*/

	ModelMatrix = translate(mat4(1.0f), Position) * toMat4(Rotation) * scale(mat4(1.0f), Scale);
	GlobalModelMatrix = translate(mat4(1.0f), GetGlobalPosition()) *
						toMat4(GetGlobalRotation()) *
						scale(mat4(1.0f), GetGlobalScale());

	m_IsDirty = false;

	// Update matrices of children 
	for (const auto child : m_Children)
		child.second->UpdateModelMatrices(true);
}

void Transform::SetModelMatrix(glm::mat4& matrix, bool global)
{
	if (m_Parent && global)
		matrix = inverse(m_Parent->GlobalModelMatrix) * matrix;

	vec3 translation, scale, skew;
	vec4 perspective;
	quat orientation;
	// Decompose(matrix, translation, euler, scale);
	glm::decompose(matrix, scale, orientation, translation, skew, perspective);

	SetPosition(translation);
	SetScale(scale);
	SetRotation(orientation);

	ModelMatrix = matrix;
	if (m_Parent) GlobalModelMatrix = m_Parent->GlobalModelMatrix * ModelMatrix;

	m_IsDirty = false;
}

mat4 Transform::GetModelMatrix(bool global)
{
	UpdateModelMatrices();
	return global ? GlobalModelMatrix : ModelMatrix;
}

#pragma region Scripting
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Transform, GetPosition, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetPosition(); }
ADD_MANAGED_METHOD(Transform, SetPosition, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->SetPosition(*input); }
ADD_MANAGED_METHOD(Transform, GetGlobalPosition, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetGlobalPosition(); }

ADD_MANAGED_METHOD(Transform, GetRotation, void, (void* handle, glm::quat* output)) { *output = ((Transform*)handle)->GetRotation(); }
ADD_MANAGED_METHOD(Transform, SetRotation, void, (void* handle, glm::quat* input)) { ((Transform*)handle)->SetRotation(*input); }
ADD_MANAGED_METHOD(Transform, GetGlobalRotation, void, (void* handle, glm::quat* output)) { *output = ((Transform*)handle)->GetGlobalRotation(); }

ADD_MANAGED_METHOD(Transform, GetScale, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetScale(); }
ADD_MANAGED_METHOD(Transform, SetScale, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->SetScale(*input); }
ADD_MANAGED_METHOD(Transform, GetGlobalScale, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetGlobalScale(); }

ADD_MANAGED_METHOD(Transform, GetUp, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Up(); }
ADD_MANAGED_METHOD(Transform, GetRight, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Right(); }
ADD_MANAGED_METHOD(Transform, GetForward, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Forward(); }

ADD_MANAGED_METHOD(Transform, GetParent, MonoObject*, (void* handle))
{
	Transform* parent = ((Transform*)handle)->GetParent();
	return parent ? parent->ManagedData.GetInstance() : nullptr;
}

ADD_MANAGED_METHOD(Transform, SetParent, void, (void* handle, void* parentHandle))
{ ((Transform*)handle)->SetParent((Transform*)parentHandle); }

ADD_MANAGED_METHOD(Transform, GetChildren, MonoArray*, (void* handle))
{
	vector<Transform*> children = ((Transform*)handle)->GetChildren();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_object_class(), children.size());
	int arrayIndex = 0;
	for(Transform* child : children)
		mono_array_set(output, MonoObject*, arrayIndex++, child->ManagedData.GetInstance());
	return output;
}

ADD_MANAGED_METHOD(Transform, AddChild, void, (void* handle, void* childHandle))
{ ((Transform*)handle)->AddChild((Transform*)childHandle); }

ADD_MANAGED_METHOD(Transform, RemoveChild, void, (void* handle, void* childHandle))
{ ((Transform*)handle)->RemoveChild((Transform*)childHandle); }
#pragma endregion
