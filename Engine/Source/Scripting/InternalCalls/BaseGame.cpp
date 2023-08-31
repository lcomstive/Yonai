#include <vector>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai::Scripting;

ADD_MANAGED_METHOD(BaseGameLauncher, LoadAssembly, bool, (MonoArray* dataRaw, MonoString* friendlyNameRaw))
{
	// Get data
	vector<unsigned char> data;
	data.resize(mono_array_length(dataRaw));
	for (size_t i = 0; i < data.size(); i++)
		data[i] = mono_array_get(dataRaw, unsigned char, i);

	// Get friendly name
	char* friendlyName = mono_string_to_utf8(friendlyNameRaw);

	Assembly* assembly = ScriptEngine::LoadAssembly(data, friendlyName);

	mono_free(friendlyName);
	return assembly != nullptr;
}