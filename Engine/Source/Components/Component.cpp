#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/Component.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

ADD_MANAGED_METHOD(NativeComponent, GetHandle, void*, (uint64_t worldID, uint64_t entityID, MonoReflectionType* type))
{
	World* world = World::GetWorld(worldID);
	if(!world)
		return nullptr;
	
	size_t typeHash = Assembly::GetTypeHash(mono_reflection_type_get_type(type));
	void* result = world->GetComponent(entityID, typeHash);
	return result;
}