#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/IO/FileWatcher.hpp>

namespace AquaEngine::Graphics
{
	struct ShaderStageInfo
	{
		std::string VertexPath;
		std::string VertexContents;

		std::string FragmentPath;
		std::string FragmentContents;

		std::string ComputePath;
		std::string ComputeContents;

		std::string GeometryPath;
		std::string GeometryContents;
	};

	struct ShaderUniform
	{		
		int Location;
		std::string Name;
		GLenum Type;
	};

	class Shader
	{
		bool m_IsDirty;
		unsigned int m_Program;
		ShaderStageInfo m_ShaderStages;
		std::vector<ShaderUniform> m_Uniforms;

		void Destroy();
		void CreateShaders();
		void CacheUniformLocations();
		void ShaderSourceChangedCallback(std::string path, IO::FileWatchStatus changeType);
		GLuint CreateShader(const std::string & source, const GLenum type, const std::string& debugName);

	public:
		AquaAPI Shader();
		AquaAPI Shader(ShaderStageInfo stageInfo);
		AquaAPI ~Shader();

		AquaAPI ShaderStageInfo& GetStages();
		AquaAPI void UpdateStages(ShaderStageInfo stageInfo);

		AquaAPI void Bind();
		AquaAPI void Unbind();

		AquaAPI unsigned int GetProgram();
		AquaAPI unsigned int GetUniformCount();

		AquaAPI void Set(int location, int value) const;
		AquaAPI void Set(int location, bool value) const;
		AquaAPI void Set(int location, float value) const;
		AquaAPI void Set(int location, double value) const;
		AquaAPI void Set(int location, glm::vec2 value) const;
		AquaAPI void Set(int location, glm::vec3 value) const;
		AquaAPI void Set(int location, glm::vec4 value) const;
		AquaAPI void Set(int location, glm::mat3 value) const;
		AquaAPI void Set(int location, glm::mat4 value) const;

		AquaAPI void Set(std::string locationName, int value) const;
		AquaAPI void Set(std::string locationName, bool value) const;
		AquaAPI void Set(std::string locationName, float value) const;
		AquaAPI void Set(std::string locationName, double value) const;
		AquaAPI void Set(std::string locationName, glm::vec2 value) const;
		AquaAPI void Set(std::string locationName, glm::vec3 value) const;
		AquaAPI void Set(std::string locationName, glm::vec4 value) const;
		AquaAPI void Set(std::string locationName, glm::mat3 value) const;
		AquaAPI void Set(std::string locationName, glm::mat4 value) const;

		/// <returns>Information about the uniform at location, or an invalid struct if outside of bounds</returns>
		AquaAPI ShaderUniform GetUniformInfo(int location) const;

		/// <returns>Information about the uniform at locationName, or an invalid struct if not found</returns>
		AquaAPI ShaderUniform GetUniformInfo(std::string locationName) const;
	};
}
