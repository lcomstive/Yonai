#include <regex>
#include <vector>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <Yonai/Graphics/Shader.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Graphics;

// Create a shader from source code
unsigned int Shader::CreateShader(const string& contents, const int type, const string& debugName)
{
	return 0;
}

Shader::Shader() : m_ShaderStages({}), m_IsDirty(false), m_Program(0) { }

Shader::Shader(ShaderStageInfo stageInfo) : m_Program(0)
{
	UpdateStages(stageInfo);
}

Shader::~Shader() { Destroy(); }

void Shader::Destroy() {}

ShaderStageInfo& Shader::GetStages() { return m_ShaderStages; }

void Shader::UpdateStages(ShaderStageInfo stageInfo)
{
	// Update & create new shaders
	m_IsDirty = true;
	m_ShaderStages = stageInfo;
	CreateShaders();
}

void Shader::ShaderSourceChangedCallback(std::string path, FileWatchStatus changeType)
{
	spdlog::debug("Shader file(s) changed, reloading..");
	m_IsDirty = true;
}

void Shader::CreateShaders()
{
	if (!m_IsDirty)
		return;

	m_IsDirty = false;
	unsigned int programID = 0;

	// Debug info
	spdlog::debug("Creating shader with following sources:");
	if (!m_ShaderStages.VertexPath.empty())   spdlog::debug("  Vertex:   {}", m_ShaderStages.VertexPath);
	if (!m_ShaderStages.FragmentPath.empty()) spdlog::debug("  Fragment: {}", m_ShaderStages.FragmentPath);
	if (!m_ShaderStages.ComputePath.empty())  spdlog::debug("  Compute:  {}", m_ShaderStages.ComputePath);
	if (!m_ShaderStages.GeometryPath.empty()) spdlog::debug("  Geometry: {}", m_ShaderStages.GeometryPath);



	// Compilation successful, destroy old program 
	Destroy();
	m_Program = programID;

	CacheUniformLocations();

	spdlog::debug("Created shader program successfully [{}]", m_Program);
}

void Shader::CacheUniformLocations()
{
	
}

void Shader::Bind() { }
void Shader::Unbind()
{
	if (m_IsDirty)
		CreateShaders();
}

unsigned int Shader::GetProgram() { return m_Program; }
unsigned int Shader::GetUniformCount() { return (unsigned int)m_Uniforms.size(); }

void Shader::Set(int location, int value) const		{}
void Shader::Set(int location, bool value) const	{}
void Shader::Set(int location, float value) const	{}
void Shader::Set(int location, double value) const	{}
void Shader::Set(int location, vec2 value) const	{}
void Shader::Set(int location, vec3 value) const	{}
void Shader::Set(int location, vec4 value) const	{}
void Shader::Set(int location, mat3 value) const	{}
void Shader::Set(int location, mat4 value) const	{}

void Shader::Set(string locationName, int value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, bool value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, float value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, double value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, vec2 value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, vec3 value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, vec4 value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, mat3 value) const { Set(GetUniformInfo(locationName).Location, value); }
void Shader::Set(string locationName, mat4 value) const { Set(GetUniformInfo(locationName).Location, value); }

ShaderUniform Shader::GetUniformInfo(int location) const
{
	return location >= 0 && location < m_Uniforms.size() ? m_Uniforms[location] : ShaderUniform{ -1 };
}

ShaderUniform Shader::GetUniformInfo(std::string locationName) const
{
	for (auto& uniform : m_Uniforms)
		if (locationName.compare(uniform.Name) == 0)
			return uniform;
	return { -1 };
}

#pragma region Managed Binding
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Shader, Load, void, (MonoString* path, uint64_t* outResourceID, void** outHandle), Yonai.Graphics)
{
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID(); // Assign resource ID
	Resource::Load<Shader>(*outResourceID, mono_string_to_utf8(path));
	*outHandle = Resource::Get<Shader>(*outResourceID);
}

ADD_MANAGED_METHOD(Shader, Bind, void, (void* instance), Yonai.Graphics)
{ ((Shader*)instance)->Bind(); }

ADD_MANAGED_METHOD(Shader, Unbind, void, (void* instance), Yonai.Graphics)
{ ((Shader*)instance)->Unbind(); }

ADD_MANAGED_METHOD(Shader, UpdateStages, void,
	(void* instance, MonoString* vertexPath, MonoString* vertexContents,
		MonoString* fragmentPath, MonoString* fragmentContents,
		MonoString* computePath, MonoString* computeContents,
		MonoString* geometryPath, MonoString* geometryContents), Yonai.Graphics)
{
	ShaderStageInfo stageInfo = {};

	stageInfo.VertexPath = vertexPath ? mono_string_to_utf8(vertexPath) : "";
	stageInfo.VertexContents = vertexContents ? mono_string_to_utf8(vertexContents) : "";

	stageInfo.FragmentPath = fragmentPath ? mono_string_to_utf8(fragmentPath) : "";
	stageInfo.FragmentContents = fragmentContents ? mono_string_to_utf8(fragmentContents) : "";

	stageInfo.ComputePath = computePath ? mono_string_to_utf8(computePath) : "";
	stageInfo.ComputeContents = computeContents ? mono_string_to_utf8(computeContents) : "";

	stageInfo.GeometryPath = geometryPath ? mono_string_to_utf8(geometryPath) : "";
	stageInfo.GeometryContents = geometryContents ? mono_string_to_utf8(geometryContents) : "";

	((Shader*)instance)->UpdateStages(stageInfo);
}

ADD_MANAGED_METHOD(Shader, UnloadStages, void, (void* instance), Yonai.Graphics)
{ ((Shader*)instance)->UpdateStages({}); }

ADD_MANAGED_METHOD(Shader, GetStages, void, (void* instance, MonoString** outVertex, MonoString** outFragment, MonoString** outCompute, MonoString** outGeometry), Yonai.Graphics)
{
	Shader* shader = (Shader*)instance;
	ShaderStageInfo& info = shader->GetStages();
	*outVertex = info.VertexPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.VertexPath.c_str());
	*outFragment = info.FragmentPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.FragmentPath.c_str());
	*outCompute = info.ComputePath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.ComputePath.c_str());
	*outGeometry = info.GeometryPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.GeometryPath.c_str());
}

ADD_MANAGED_METHOD(Shader, Set_int, void, (void* instance, int location, int value), Yonai.Graphics)
{ ((Shader*)instance)->Set(location, value); }
ADD_MANAGED_METHOD(Shader, SetStr_int, void, (void* instance, MonoString* location, int value), Yonai.Graphics)
{ ((Shader*)instance)->Set(mono_string_to_utf8(location), value); }

ADD_MANAGED_METHOD(Shader, Set_bool, void, (void* instance, int location, bool value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_bool, void, (void* instance, MonoString* location, bool value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_float, void, (void* instance, int location, float value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_float, void, (void* instance, MonoString* location, float value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_double, void, (void* instance, int location, double value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_double, void, (void* instance, MonoString* location, double value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec2, void, (void* instance, int location, glm::vec2* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec2, void, (void* instance, MonoString* location, glm::vec2* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec3, void, (void* instance, int location, glm::vec3* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec3, void, (void* instance, MonoString* location, glm::vec3* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec4, void, (void* instance, int location, glm::vec4* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec4, void, (void* instance, MonoString* location, glm::vec4* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_mat3, void, (void* instance, int location, glm::mat3* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_mat3, void, (void* instance, MonoString* location, glm::mat3* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_mat4, void, (void* instance, int location, glm::mat4* value), Yonai.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_mat4, void, (void* instance, MonoString* location, glm::mat4* value), Yonai.Graphics)
{ ((Shader*)instance)->Set(mono_string_to_utf8(location), value); }

#pragma endregion