#include <AquaEngine/World.hpp>
#include <glm/gtx/quaternion.hpp>
#include <AquaEngine/Components/Transform.hpp>

using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

vec3 Transform::GetGlobalPosition()
{
	return Position + (Parent ? Parent->GetGlobalPosition() : vec3(0));
}

quat Transform::GetGlobalRotation()
{
	if (Parent)
		return Parent->GetGlobalRotation() * Rotation;
	else
		return Rotation;
}

vec3 Transform::GetGlobalScale()
{
	return Scale * (Parent ? Parent->GetGlobalScale() : vec3(1.0f));
}

vec3 Transform::Up()	  { return vec3(0, 1, 0) * Rotation; }
vec3 Transform::Right()	  { return vec3(1, 0, 0) * Rotation; }
vec3 Transform::Forward() { return vec3(0, 0, 1) * Rotation; }

mat4 Transform::GetModelMatrix()
{
	// Generate model matrix
	mat4 translationMatrix = translate(mat4(1.0f), Position);
	mat4 scaleMatrix = scale(mat4(1.0f), Scale);
	mat4 rotationMatrix = toMat4(Rotation);
	
	mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	if (Parent)
		modelMatrix = Parent->GetModelMatrix() * modelMatrix;

	return modelMatrix;
}

void Transform::SetParent(Transform* parent)
{
	if (Parent)
		Parent->RemoveChild(this);

	Parent = parent;

	if (Parent)
		Parent->AddChild(this);
}

void Transform::AddChild(Transform* child)
{
	UUID id = child->Entity.ID();
	if (Children.find(id) == Children.end())
		Children.emplace(id, child);
	child->Parent = this;
}

void Transform::RemoveChild(Transform* child)
{
	UUID id = child->Entity.ID();
	if (Children.find(id) != Children.end())
		Children.erase(id);
	child->Parent = nullptr;
}

#pragma region Scripting
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Transform, GetPosition, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->Position; }
ADD_MANAGED_METHOD(Transform, SetPosition, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->Position = *input; }
ADD_MANAGED_METHOD(Transform, GetGlobalPosition, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetGlobalPosition(); }

ADD_MANAGED_METHOD(Transform, GetRotation, void, (void* handle, glm::quat* output)) { *output = ((Transform*)handle)->Rotation; }
ADD_MANAGED_METHOD(Transform, SetRotation, void, (void* handle, glm::quat* input)) { ((Transform*)handle)->Rotation = *input; }
ADD_MANAGED_METHOD(Transform, GetGlobalRotation, void, (void* handle, glm::quat* output)) { *output = ((Transform*)handle)->GetGlobalRotation(); }

ADD_MANAGED_METHOD(Transform, GetScale, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->Scale; }
ADD_MANAGED_METHOD(Transform, SetScale, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->Scale = *input; }
ADD_MANAGED_METHOD(Transform, GetGlobalScale, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->GetGlobalScale(); }

ADD_MANAGED_METHOD(Transform, GetUp, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Up(); }
ADD_MANAGED_METHOD(Transform, GetRight, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Right(); }
ADD_MANAGED_METHOD(Transform, GetForward, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Forward(); }

ADD_MANAGED_METHOD(Transform, GetParent, MonoObject*, (void* handle))
{
	Transform* parent = ((Transform*)handle)->Parent;
	return parent ? parent->ManagedData.GetInstance() : nullptr;
}

ADD_MANAGED_METHOD(Transform, SetParent, void, (void* handle, void* parentHandle))
{ ((Transform*)handle)->SetParent((Transform*)parentHandle); }

ADD_MANAGED_METHOD(Transform, GetChildren, MonoArray*, (void* handle))
{
	auto children = ((Transform*)handle)->Children;
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_object_class(), children.size());
	int arrayIndex = 0;
	for(auto child : children)
		mono_array_set(output, MonoObject*, arrayIndex++, child.second->ManagedData.GetInstance());
	return output;
}

ADD_MANAGED_METHOD(Transform, AddChild, void, (void* handle, void* childHandle))
{ ((Transform*)handle)->AddChild((Transform*)childHandle); }

ADD_MANAGED_METHOD(Transform, RemoveChild, void, (void* handle, void* childHandle))
{ ((Transform*)handle)->RemoveChild((Transform*)childHandle); }
#pragma endregion
