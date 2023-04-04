#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine::IO;

ADD_MANAGED_METHOD(VFS, GetCurrentDirectory, MonoString*)
{ return mono_string_new(mono_domain_get(), VFS::GetCurrentDirectory().string().c_str()); }

ADD_MANAGED_METHOD(VFS, HasMount, bool, (MonoString* path))
{ return VFS::HasMount(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, Mount, void, (MonoString* mountPoint))
{ VFS::Mount(mono_string_to_utf8(mountPoint)); }

ADD_MANAGED_METHOD(VFS, MountPath, void, (MonoString* mountPoint, MonoString* mountPath))
{ VFS::Mount(mono_string_to_utf8(mountPoint), mono_string_to_utf8(mountPath)); }

ADD_MANAGED_METHOD(VFS, Unmount, void, (MonoString* mountPoint))
{ VFS::Unmount(mono_string_to_utf8(mountPoint)); }

ADD_MANAGED_METHOD(VFS, UnmountPath, void, (MonoString* mountPoint, MonoString* mountPath))
{ VFS::Unmount(mono_string_to_utf8(mountPoint), mono_string_to_utf8(mountPath)); }

ADD_MANAGED_METHOD(VFS, CreateDirectory, bool, (MonoString* path))
{ return VFS::CreateDirectory(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, DeleteDirectory, bool, (MonoString* path))
{ return VFS::DeleteDirectory(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, ReadText, MonoString*, (MonoString* path))
{
	string content = VFS::ReadText(mono_string_to_utf8(path));
	return mono_string_new(mono_domain_get(), content.c_str());
}

ADD_MANAGED_METHOD(VFS, WriteText, void, (MonoString* path, MonoString* content))
{ VFS::WriteText(mono_string_to_utf8(path), mono_string_to_utf8(content)); }

ADD_MANAGED_METHOD(VFS, Read, MonoArray*, (MonoString* path))
{
	vector<unsigned char> data = VFS::Read(mono_string_to_utf8(path));

	MonoArray* array = mono_array_new(mono_domain_get(), mono_get_byte_class(), data.size());
	for(size_t i = 0; i < data.size(); i++)
		mono_array_set(array, unsigned char, i, data[i]);
	return array;
}

ADD_MANAGED_METHOD(VFS, Write, void, (MonoString* path, MonoArray* array))
{
	size_t dataLength = mono_array_length(array);
	vector<unsigned char> data;
	data.reserve(dataLength);

	for(size_t i = 0; i < dataLength; i++)
		data.emplace_back(mono_array_get(array, unsigned char, i));

	VFS::Write(mono_string_to_utf8(path), data);
}

ADD_MANAGED_METHOD(VFS, Remove, void, (MonoString* path))
{ VFS::Remove(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, Move, void, (MonoString* from, MonoString* to))
{ VFS::Move(mono_string_to_utf8(from), mono_string_to_utf8(to)); }

ADD_MANAGED_METHOD(VFS, Copy, void, (MonoString* from, MonoString* to))
{ VFS::Copy(mono_string_to_utf8(from), mono_string_to_utf8(to)); }

ADD_MANAGED_METHOD(VFS, GetAbsolutePath, MonoString*, (MonoString* path, bool requireFileExists, bool suppressWarnings))
{ return mono_string_new(mono_domain_get(), VFS::GetAbsolutePath(mono_string_to_utf8(path), requireFileExists, suppressWarnings).c_str()); }

ADD_MANAGED_METHOD(VFS, Exists, bool, (MonoString* path))
{ return VFS::Exists(mono_string_to_utf8(path)); }
