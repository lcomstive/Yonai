#include <regex>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Graphics/Shader.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;

// Replace all instances of '#include "<path>"' with contents of file at path
void ProcessIncludeFiles(string& text)
{
	// regex expression for pattern to be searched
	regex regexp("\\#include\\s+\\\"(.*)\\\"");
	smatch match;

	// regex_search that searches pattern regexp in the string mystr
	while (regex_search(text, match, regexp))
	{
		// match.str(0) is the entire captured string
		// match.str(1) is first capture group, in this case filename

		text = text.replace(
			match.prefix().length(),
			match.length(),
			VFS::ReadText(match.str(1))
		);
	}
}

// Create a shader from source code
GLuint Shader::CreateShader(const string& source, const GLenum type, const string& debugName)
{
	if (source.empty())
		return GL_INVALID_VALUE;

	// Create shader object
	const GLuint shader = glCreateShader(type);

	// Add source text & compile
	string readSource = VFS::ReadText(source);
	if (readSource.empty())
		return GL_INVALID_VALUE;

	ProcessIncludeFiles(readSource);

	const char* sourceRaw = readSource.c_str();
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

Shader::~Shader()
{
	WatchShader(m_ShaderStages.VertexPath, false);
	WatchShader(m_ShaderStages.FragmentPath, false);
	WatchShader(m_ShaderStages.GeometryPath, false);
	WatchShader(m_ShaderStages.ComputePath, false);

	Destroy();
}

void Shader::Destroy()
{
	if (m_Program != GL_INVALID_VALUE)
		glDeleteProgram(m_Program);
	m_Program = GL_INVALID_VALUE;
}

ShaderStageInfo& Shader::GetStages() { return m_ShaderStages; }

void Shader::WatchShader(string path, bool watch)
{
	if (path.empty())
		return;
	VFSMapping* mapping = VFS::GetMapping(path);
	if (!mapping)
	{
		spdlog::warn("Can't watch shader '{} - no VFS mapping was found", path);
		return;
	}

	if (watch)
		mapping->Watch(path, std::bind(&Shader::ShaderSourceChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
	else
		mapping->Unwatch(path);
}

void Shader::UpdateStages(ShaderStageInfo stageInfo)
{
	// Check for same stages, in that case won't need to recreate shader program
	if (stageInfo.VertexPath.compare(m_ShaderStages.VertexPath) == 0 &&
		stageInfo.FragmentPath.compare(m_ShaderStages.FragmentPath) == 0 &&
		stageInfo.GeometryPath.compare(m_ShaderStages.GeometryPath) == 0 &&
		stageInfo.ComputePath.compare(m_ShaderStages.ComputePath) == 0)
		return;

	// Unwatch shaders
	WatchShader(m_ShaderStages.VertexPath, false);
	WatchShader(m_ShaderStages.FragmentPath, false);
	WatchShader(m_ShaderStages.GeometryPath, false);
	WatchShader(m_ShaderStages.ComputePath, false);

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
		CreateShader(m_ShaderStages.VertexPath, GL_VERTEX_SHADER, "vertex"),
		CreateShader(m_ShaderStages.FragmentPath, GL_FRAGMENT_SHADER, "fragment"),
		CreateShader(m_ShaderStages.ComputePath, GL_COMPUTE_SHADER, "compute"),
		CreateShader(m_ShaderStages.GeometryPath, GL_GEOMETRY_SHADER, "geometry")
	};

	// Debug info
	spdlog::debug("Creating shader with following sources:");
	if (!m_ShaderStages.VertexPath.empty())   spdlog::debug("  Vertex:   {}", m_ShaderStages.VertexPath);
	if (!m_ShaderStages.FragmentPath.empty()) spdlog::debug("  Fragment: {}", m_ShaderStages.FragmentPath);
	if (!m_ShaderStages.ComputePath.empty())  spdlog::debug("  Compute:  {}", m_ShaderStages.ComputePath);
	if (!m_ShaderStages.GeometryPath.empty()) spdlog::debug("  Geometry: {}", m_ShaderStages.GeometryPath);

	WatchShader(m_ShaderStages.VertexPath);
	WatchShader(m_ShaderStages.FragmentPath);
	WatchShader(m_ShaderStages.GeometryPath);
	WatchShader(m_ShaderStages.ComputePath);

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