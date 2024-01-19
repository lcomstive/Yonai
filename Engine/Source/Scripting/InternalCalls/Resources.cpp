#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Scripting;

ADD_MANAGED_METHOD(Resource, GetPath, MonoString*, (uint64_t id))
{
	if (Resource::IsValidResourceID(id))
		return mono_string_new(mono_domain_get(), Resource::GetPath(id).c_str());
	return nullptr;
}

ADD_MANAGED_METHOD(Resource, GetID, uint64_t, (MonoString* path))
{ return path ? Resource::GetID(mono_string_to_utf8(path)) : InvalidResourceID; }

ADD_MANAGED_METHOD(Resource, CreateID, uint64_t, ())
{ return UUID(); }

ADD_MANAGED_METHOD(Resource, Unload, void, (uint64_t resourceID))
{ Resource::Unload(resourceID); }

ADD_MANAGED_METHOD(Resource, GetInstance, void*, (uint64_t resourceID))
{ return Resource::IsValidResourceID(resourceID) ? Resource::Get(resourceID) : nullptr; }

ADD_MANAGED_METHOD(Resource, Exists, bool, (uint64_t resourceID))
{ return Resource::IsValidResourceID(resourceID); }

ADD_MANAGED_METHOD(Resource, Print)
{ Resource::PrintResourceTypes(); }