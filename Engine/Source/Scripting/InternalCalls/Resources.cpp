#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

ADD_MANAGED_METHOD(Resource, Duplicate, uint64_t, (unsigned int originalID, MonoString* newPath))
{
	return Resource::IsValidResourceID(originalID) ?
		Resource::Duplicate(originalID, mono_string_to_utf8(newPath)) : InvalidResourceID;
}

ADD_MANAGED_METHOD(Resource, GetPath, MonoString*, (unsigned int id))
{
	if (Resource::IsValidResourceID(id))
		return mono_string_new(mono_domain_get(), Resource::GetPath(id).c_str());
	return nullptr;
}

ADD_MANAGED_METHOD(Resource, GetID, uint64_t, (MonoString* path))
{ return Resource::GetID(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(Resource, CreateID, uint64_t, ())
{ return UUID(); }

ADD_MANAGED_METHOD(Resource, Unload, void, (uint64_t resourceID))
{ Resource::Unload(resourceID); }

ADD_MANAGED_METHOD(Resource, GetInstance, void*, (uint64_t resourceID))
{ return Resource::IsValidResourceID(resourceID) ? Resource::Get(resourceID) : nullptr; }

ADD_MANAGED_METHOD(Resource, Exists, bool, (uint64_t resourceID))
{ return Resource::IsValidResourceID(resourceID); }