#include <string>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <portable-file-dialogs/portable-file-dialogs.h>

using namespace std;

ADD_MANAGED_METHOD(FileDialog, _OpenFolder, MonoString*, (MonoString* titleRaw, MonoString* initialDirRaw), AquaEditor)
{
	char* title = mono_string_to_utf8(titleRaw);
	char* initialDir = mono_string_to_utf8(initialDirRaw);
	string selected = pfd::select_folder(title, initialDir).result();

	mono_free(title);
	mono_free(initialDir);

	return mono_string_new(mono_domain_get(), selected.c_str());
}

ADD_MANAGED_METHOD(FileDialog, _OpenFile, MonoArray*, (MonoString* titleRaw, MonoString* initialDirRaw, MonoArray* filtersRaw, bool multiselect), AquaEditor)
{
	string title = string(mono_string_to_utf8(titleRaw));
	string initialDir = string(mono_string_to_utf8(initialDirRaw));

	vector<string> filters;
	filters.resize(mono_array_length(filtersRaw));
	for (size_t i = 0; i < filters.size(); i++)
		filters[i] = string(mono_string_to_utf8(mono_array_get(filtersRaw, MonoString*, i)));

	vector<string> selected = pfd::open_file(title, initialDir, filters, multiselect).result();

	mono_free(title.data());
	mono_free(initialDir.data());

	for (string filter : filters)
		mono_free(filter.data());
	filters.clear();

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_string_class(), selected.size());
	for (size_t i = 0; i < selected.size(); i++)
		mono_array_set(output, MonoString*, i, mono_string_new(mono_domain_get(), selected[i].c_str()));
	return output;
}