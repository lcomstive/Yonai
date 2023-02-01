#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

ADD_MANAGED_METHOD(Resource, GetType, MonoType*, (unsigned int resourceID))
{
	if (!Resource::IsValidResourceID(resourceID))
		return nullptr;
	type_index type = Resource::GetType(resourceID);
	return Assembly::GetTypeFromHash(type.hash_code());
}

ADD_MANAGED_METHOD(Resource, Duplicate, unsigned int, (unsigned int originalID, MonoString* newPath))
{
	return Resource::IsValidResourceID(originalID) ?
		Resource::Duplicate(originalID, mono_string_to_utf8(newPath)) : InvalidResourceID;
}

ADD_MANAGED_METHOD(Resource, GetPath, MonoString*, (unsigned int id))
{
	return mono_string_new(mono_domain_get(),
		(Resource::IsValidResourceID(id) ? Resource::GetPath(id) : string()).c_str());
}

ADD_MANAGED_METHOD(Resource, GetID, unsigned int, (MonoString* path))
{ return Resource::GetID(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(Resource, Unload, void, (unsigned int resourceID))
{ Resource::Unload(resourceID); }

ADD_MANAGED_METHOD(Resource, GetInstance, void*, (unsigned int resourceID))
{ return Resource::IsValidResourceID(resourceID) ? Resource::Get(resourceID) : nullptr; }
