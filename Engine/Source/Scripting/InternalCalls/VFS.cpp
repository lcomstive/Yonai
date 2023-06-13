#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine::IO;

ADD_MANAGED_METHOD(VFS, GetCurrentDirectory, MonoString*, (), AquaEngine.IO)
{ return mono_string_new(mono_domain_get(), VFS::GetCurrentDirectory().string().c_str()); }

ADD_MANAGED_METHOD(VFS, HasMount, bool, (MonoString* path), AquaEngine.IO)
{ return VFS::HasMount(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, Mount, void, (MonoString* mountPoint), AquaEngine.IO)
{ VFS::Mount(mono_string_to_utf8(mountPoint)); }

ADD_MANAGED_METHOD(VFS, MountPath, void, (MonoString* mountPoint, MonoString* mountPath), AquaEngine.IO)
{ VFS::Mount(mono_string_to_utf8(mountPoint), mono_string_to_utf8(mountPath)); }

ADD_MANAGED_METHOD(VFS, Unmount, void, (MonoString* mountPoint), AquaEngine.IO)
{ VFS::Unmount(mono_string_to_utf8(mountPoint)); }

ADD_MANAGED_METHOD(VFS, UnmountPath, void, (MonoString* mountPoint, MonoString* mountPath), AquaEngine.IO)
{ VFS::Unmount(mono_string_to_utf8(mountPoint), mono_string_to_utf8(mountPath)); }

ADD_MANAGED_METHOD(VFS, CreateDirectory, bool, (MonoString* path), AquaEngine.IO)
{ return VFS::CreateDirectory(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, DeleteDirectory, bool, (MonoString* path), AquaEngine.IO)
{ return VFS::DeleteDirectory(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, ReadText, MonoString*, (MonoString* path), AquaEngine.IO)
{
	string content = VFS::ReadText(mono_string_to_utf8(path));
	return mono_string_new(mono_domain_get(), content.c_str());
}

ADD_MANAGED_METHOD(VFS, WriteText, void, (MonoString* path, MonoString* content), AquaEngine.IO)
{ VFS::WriteText(mono_string_to_utf8(path), mono_string_to_utf8(content)); }

ADD_MANAGED_METHOD(VFS, Read, MonoArray*, (MonoString* path), AquaEngine.IO)
{
	vector<unsigned char> data = VFS::Read(mono_string_to_utf8(path));

	MonoArray* array = mono_array_new(mono_domain_get(), mono_get_byte_class(), data.size());
	for(size_t i = 0; i < data.size(); i++)
		mono_array_set(array, unsigned char, i, data[i]);
	return array;
}

ADD_MANAGED_METHOD(VFS, Write, void, (MonoString* path, MonoArray* array), AquaEngine.IO)
{
	size_t dataLength = mono_array_length(array);
	vector<unsigned char> data;
	data.reserve(dataLength);

	for(size_t i = 0; i < dataLength; i++)
		data.emplace_back(mono_array_get(array, unsigned char, i));

	VFS::Write(mono_string_to_utf8(path), data);
}

ADD_MANAGED_METHOD(VFS, Remove, void, (MonoString* path), AquaEngine.IO)
{ VFS::Remove(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, Move, void, (MonoString* from, MonoString* to), AquaEngine.IO)
{ VFS::Move(mono_string_to_utf8(from), mono_string_to_utf8(to)); }

ADD_MANAGED_METHOD(VFS, Copy, void, (MonoString* from, MonoString* to), AquaEngine.IO)
{ VFS::Copy(mono_string_to_utf8(from), mono_string_to_utf8(to)); }

ADD_MANAGED_METHOD(VFS, GetAbsolutePath, MonoString*, (MonoString* path, bool requireFileExists, bool suppressWarnings), AquaEngine.IO)
{ return mono_string_new(mono_domain_get(), VFS::GetAbsolutePath(mono_string_to_utf8(path), requireFileExists, suppressWarnings).c_str()); }

ADD_MANAGED_METHOD(VFS, Exists, bool, (MonoString* path), AquaEngine.IO)
{ return VFS::Exists(mono_string_to_utf8(path)); }

ADD_MANAGED_METHOD(VFS, GetFiles, MonoArray*, (MonoString* directoryRaw, bool recursive), AquaEngine.IO)
{
	char* directory = mono_string_to_utf8(directoryRaw);
	vector<VFSFile> files = VFS::GetFiles(directory, recursive);

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_string_class(), (uintptr_t)files.size());
	for (size_t i = 0; i < files.size(); i++)
	{
		string path = files[i].FullPath();
		if (files[i].Directory)
			path += "/";
		mono_array_set(output, MonoString*, i, mono_string_new(mono_domain_get(), path.c_str()));
	}

	mono_free(directory);
	return output;
}