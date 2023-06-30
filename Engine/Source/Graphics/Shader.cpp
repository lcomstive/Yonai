#include <regex>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <AquaEngine/Graphics/Shader.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;

// Create a shader from source code
GLuint Shader::CreateShader(const string& contents, const GLenum type, const string& debugName)
{
	if (contents.empty())
		return GL_INVALID_VALUE;

	// Create shader object
	const GLuint shader = glCreateShader(type);

	const char* sourceRaw = contents.c_str();
	glShaderSource(shader, 1, &sourceRaw, nullptr);
	glCompileShader(shader);

	// Check success status
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		// Get and print error message
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(shader);

		spdlog::error("Failed to compile '{} - {}", debugName, infoLog.data());
		return GL_INVALID_VALUE;
	}
	return shader;
}

Shader::Shader() : m_ShaderStages({}), m_IsDirty(false), m_Program(GL_INVALID_VALUE) { }

Shader::Shader(ShaderStageInfo stageInfo) : m_Program(GL_INVALID_VALUE)
{
	UpdateStages(stageInfo);
}

Shader::~Shader() { Destroy(); }

void Shader::Destroy()
{
	if (m_Program != GL_INVALID_VALUE)
		glDeleteProgram(m_Program);
	m_Program = GL_INVALID_VALUE;
}

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

	vector<GLuint> shaders =
	{
		CreateShader(m_ShaderStages.VertexContents, GL_VERTEX_SHADER, "vertex"),
		CreateShader(m_ShaderStages.FragmentContents, GL_FRAGMENT_SHADER, "fragment"),
		CreateShader(m_ShaderStages.ComputeContents, GL_COMPUTE_SHADER, "compute"),
		CreateShader(m_ShaderStages.GeometryContents, GL_GEOMETRY_SHADER, "geometry")
	};

	// Debug info
	spdlog::debug("Creating shader with following sources:");
	if (!m_ShaderStages.VertexPath.empty())   spdlog::debug("  Vertex:   {}", m_ShaderStages.VertexPath);
	if (!m_ShaderStages.FragmentPath.empty()) spdlog::debug("  Fragment: {}", m_ShaderStages.FragmentPath);
	if (!m_ShaderStages.ComputePath.empty())  spdlog::debug("  Compute:  {}", m_ShaderStages.ComputePath);
	if (!m_ShaderStages.GeometryPath.empty()) spdlog::debug("  Geometry: {}", m_ShaderStages.GeometryPath);

	const bool invalidShaders = shaders.size() == 0 || shaders[0] == GL_INVALID_VALUE || shaders[1] == GL_INVALID_VALUE;
	programID = invalidShaders ? GL_INVALID_VALUE : glCreateProgram();

	for (GLuint shader : shaders)
	{
		if (shader == GL_INVALID_VALUE)
			continue;
		if (invalidShaders)
			glDeleteShader(shader);
		else
			glAttachShader(programID, shader);
	}

	if (invalidShaders)
		return;

	// Finalise shader program
	glLinkProgram(programID);

	// Check success status
	int success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		// Get and print error message
		int maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		vector<char> infoLog(maxLength);
		glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

		// Clean up resources
		glDeleteProgram(programID);
		for (GLuint shader : shaders)
			if (shader != GL_INVALID_VALUE)
				glDeleteShader(shader);

		spdlog::error("Failed to link shader program - {}", infoLog.data());
		return;
	}

	// The shaders aren't needed once they've been linked into the program
	for (GLuint shader : shaders)
		if (shader != GL_INVALID_VALUE)
			glDeleteShader(shader);

	// Compilation successful, destroy old program 
	Destroy();
	m_Program = programID;

	CacheUniformLocations();

	spdlog::debug("Created shader program successfully [{}]", m_Program);
}

void Shader::CacheUniformLocations()
{
	int uniformCount = 0;
	glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &uniformCount);

	spdlog::debug("Caching {} shader uniforms", uniformCount);
	m_Uniforms.clear();
	m_Uniforms.resize(uniformCount);

	for (GLuint i = 0; i < (GLuint)uniformCount; i++)
	{
		GLint nameLength;
		glGetActiveUniformsiv(m_Program, 1, &i, GL_UNIFORM_NAME_LENGTH, &nameLength);

		m_Uniforms[i].Name.resize(nameLength - 1);

		GLint size;
		glGetActiveUniform(m_Program, i, nameLength, &nameLength, &size, &m_Uniforms[i].Type, m_Uniforms[i].Name.data());

		m_Uniforms[i].Location = glGetUniformLocation(m_Program, m_Uniforms[i].Name.c_str());

		spdlog::debug(" [{}] {}", m_Uniforms[i].Location, m_Uniforms[i].Name);
	}
}

void Shader::Bind() { if (m_Program != GL_INVALID_VALUE) glUseProgram(m_Program); }
void Shader::Unbind()
{
	glUseProgram(0);

	if (m_IsDirty)
		CreateShaders();
}

unsigned int Shader::GetProgram() { return m_Program; }
unsigned int Shader::GetUniformCount() { return (unsigned int)m_Uniforms.size(); }

void Shader::Set(int location, int value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform1i(m_Program, location, value); }
void Shader::Set(int location, bool value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform1i(m_Program, location, value); }
void Shader::Set(int location, float value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform1f(m_Program, location, value); }
void Shader::Set(int location, double value) const { Set(location, (float)value); }
void Shader::Set(int location, vec2 value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform2f(m_Program, location, value.x, value.y); }
void Shader::Set(int location, vec3 value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform3f(m_Program, location, value.x, value.y, value.z); }
void Shader::Set(int location, vec4 value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniform4f(m_Program, location, value.x, value.y, value.z, value.w); }
void Shader::Set(int location, mat3 value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniformMatrix3fv(m_Program, location, 1, GL_FALSE, value_ptr(value)); }
void Shader::Set(int location, mat4 value) const { if (m_Program != GL_INVALID_VALUE) glProgramUniformMatrix4fv(m_Program, location, 1, GL_FALSE, value_ptr(value)); }

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
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Shader, Load, void, (MonoString* path, uint64_t* outResourceID, void** outHandle), AquaEngine.Graphics)
{
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID(); // Assign resource ID
	Resource::Load<Shader>(*outResourceID, mono_string_to_utf8(path));
	*outHandle = Resource::Get<Shader>(*outResourceID);
}

ADD_MANAGED_METHOD(Shader, Bind, void, (void* instance), AquaEngine.Graphics)
{ ((Shader*)instance)->Bind(); }

ADD_MANAGED_METHOD(Shader, Unbind, void, (void* instance), AquaEngine.Graphics)
{ ((Shader*)instance)->Unbind(); }

ADD_MANAGED_METHOD(Shader, UpdateStages, void,
	(void* instance, MonoString* vertexPath, MonoString* vertexContents,
		MonoString* fragmentPath, MonoString* fragmentContents,
		MonoString* computePath, MonoString* computeContents,
		MonoString* geometryPath, MonoString* geometryContents), AquaEngine.Graphics)
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

ADD_MANAGED_METHOD(Shader, UnloadStages, void, (void* instance), AquaEngine.Graphics)
{ ((Shader*)instance)->UpdateStages({}); }

ADD_MANAGED_METHOD(Shader, GetStages, void, (void* instance, MonoString** outVertex, MonoString** outFragment, MonoString** outCompute, MonoString** outGeometry), AquaEngine.Graphics)
{
	Shader* shader = (Shader*)instance;
	ShaderStageInfo& info = shader->GetStages();
	*outVertex = info.VertexPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.VertexPath.c_str());
	*outFragment = info.FragmentPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.FragmentPath.c_str());
	*outCompute = info.ComputePath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.ComputePath.c_str());
	*outGeometry = info.GeometryPath.empty() ? nullptr : mono_string_new(mono_domain_get(), info.GeometryPath.c_str());
}

ADD_MANAGED_METHOD(Shader, Set_int, void, (void* instance, int location, int value), AquaEngine.Graphics)
{ ((Shader*)instance)->Set(location, value); }
ADD_MANAGED_METHOD(Shader, SetStr_int, void, (void* instance, MonoString* location, int value), AquaEngine.Graphics)
{ ((Shader*)instance)->Set(mono_string_to_utf8(location), value); }

ADD_MANAGED_METHOD(Shader, Set_bool, void, (void* instance, int location, bool value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_bool, void, (void* instance, MonoString* location, bool value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_float, void, (void* instance, int location, float value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_float, void, (void* instance, MonoString* location, float value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_double, void, (void* instance, int location, double value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_double, void, (void* instance, MonoString* location, double value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec2, void, (void* instance, int location, glm::vec2* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec2, void, (void* instance, MonoString* location, glm::vec2* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec3, void, (void* instance, int location, glm::vec3* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec3, void, (void* instance, MonoString* location, glm::vec3* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_vec4, void, (void* instance, int location, glm::vec4* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_vec4, void, (void* instance, MonoString* location, glm::vec4* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_mat3, void, (void* instance, int location, glm::mat3* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_mat3, void, (void* instance, MonoString* location, glm::mat3* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(mono_string_to_utf8(location), value);
}

ADD_MANAGED_METHOD(Shader, Set_mat4, void, (void* instance, int location, glm::mat4* value), AquaEngine.Graphics)
{
	((Shader*)instance)->Set(location, value);
}
ADD_MANAGED_METHOD(Shader, SetStr_mat4, void, (void* instance, MonoString* location, glm::mat4* value), AquaEngine.Graphics)
{ ((Shader*)instance)->Set(mono_string_to_utf8(location), value); }

#pragma endregion