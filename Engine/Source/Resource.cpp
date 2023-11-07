#include <Yonai/Resource.hpp>

using namespace std;
using namespace Yonai;

vector<Resource::ResourceInstance> Resource::s_Instances;
unordered_map<string, ResourceID> Resource::s_InstancePaths;
unordered_map<ResourceID, size_t> Resource::s_ResourceIDs;

string Resource::GetPath(ResourceID id)
{
	for (auto it = s_InstancePaths.begin(); it != s_InstancePaths.end(); it++)
		if (it->second == id)
			return it->first;
	return "";
}

ResourceID Resource::GetID(const string& path)
{
	return s_InstancePaths.find(path) == s_InstancePaths.end() ?
		InvalidResourceID : s_InstancePaths[path];
}

void* Resource::Get(ResourceID id)
{
	ResourceInstance* instance = GetInstance(id);
	return instance ? instance->Data : nullptr;
}

void* Resource::Get(string path)
{
	if (!Exists(path))
		return nullptr;
	ResourceInstance* instance = GetInstance(s_InstancePaths[path]);
	return instance ? instance->Data : nullptr;
}

void Resource::Unload(ResourceID resource)
{
	if (!IsValidResourceID(resource))
	{
		spdlog::error("Failed to unload resource [{}] - invalid ID", resource);
		return;
	}

	// Unload data & remove from instances
	size_t instanceIndex = s_ResourceIDs[resource];

	// Call destructor if inherits from ResourceBase
	ResourceInstance& resourceInstance = s_Instances[instanceIndex];
	if (resourceInstance.IsResourceBaseType)
		((ResourceBase*)resourceInstance.Data)->~ResourceBase();

	delete s_Instances[instanceIndex].Data;
	s_Instances[instanceIndex].Data = nullptr;

	// Put last instance into removed index, then erase the last element.
	// This is done because with a traditional vector::erase all elements get relocated,
	//	moving the final element to override the erased instance provides more efficiency
	size_t lastIndex = s_Instances.size() - 1;
	if (instanceIndex < lastIndex)
	{
		// Move last instance to override erased one
		s_Instances[instanceIndex] = s_Instances[lastIndex];

		// Update ResourceID<->InstanceIndex mapping
		for (auto& it : s_ResourceIDs)
		{
			if (it.second == lastIndex)
			{
				s_ResourceIDs[it.first] = instanceIndex;
				break;
			}
		}
	}

	s_Instances.pop_back();

	// Remove resource ID from pool
	s_ResourceIDs.erase(resource);

	// Remove from instance paths
	for (auto it = s_InstancePaths.begin(); it != s_InstancePaths.end(); ++it)
	{
		if (it->second == resource)
		{
			s_InstancePaths.erase(it);
			break;
		}
	}
}

void Resource::UnloadAll()
{
	for (auto& instance : s_Instances)
		delete instance.Data;
	s_Instances.clear();
	s_ResourceIDs.clear();
	s_InstancePaths.clear();
}

Resource::ResourceInstance* Resource::GetInstance(ResourceID id)
{
	return IsValidResourceID(id) ? &s_Instances[s_ResourceIDs[id]] : nullptr;
}

bool Resource::Exists(string path)
{
	return s_InstancePaths.find(path) != s_InstancePaths.end();
}

bool Resource::IsValidResourceID(ResourceID id)
{
	return id != InvalidResourceID &&
		s_ResourceIDs.find(id) != s_ResourceIDs.end();
}

void Resource::PrintResourceTypes()
{
	spdlog::trace("Printing resources:");
	for (auto& pair : s_InstancePaths)
	{
		ResourceInstance* instance = GetInstance(pair.second);
		spdlog::trace(" [{}->{}][{}] \t{}", pair.second, s_ResourceIDs[pair.second], instance->Type.name(), pair.first);
	}
}

type_index Resource::GetType(ResourceID id)
{
	ResourceInstance* instance = GetInstance(id);
	return instance ? instance->Type : typeid(void);
}
