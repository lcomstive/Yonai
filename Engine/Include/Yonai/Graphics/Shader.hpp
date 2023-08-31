#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <Yonai/Application.hpp>
#include <Yonai/IO/FileWatcher.hpp>

namespace Yonai::Graphics
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
		YonaiAPI Shader();
		YonaiAPI Shader(ShaderStageInfo stageInfo);
		YonaiAPI ~Shader();

		YonaiAPI ShaderStageInfo& GetStages();
		YonaiAPI void UpdateStages(ShaderStageInfo stageInfo);

		YonaiAPI void Bind();
		YonaiAPI void Unbind();

		YonaiAPI unsigned int GetProgram();
		YonaiAPI unsigned int GetUniformCount();

		YonaiAPI void Set(int location, int value) const;
		YonaiAPI void Set(int location, bool value) const;
		YonaiAPI void Set(int location, float value) const;
		YonaiAPI void Set(int location, double value) const;
		YonaiAPI void Set(int location, glm::vec2 value) const;
		YonaiAPI void Set(int location, glm::vec3 value) const;
		YonaiAPI void Set(int location, glm::vec4 value) const;
		YonaiAPI void Set(int location, glm::mat3 value) const;
		YonaiAPI void Set(int location, glm::mat4 value) const;

		YonaiAPI void Set(std::string locationName, int value) const;
		YonaiAPI void Set(std::string locationName, bool value) const;
		YonaiAPI void Set(std::string locationName, float value) const;
		YonaiAPI void Set(std::string locationName, double value) const;
		YonaiAPI void Set(std::string locationName, glm::vec2 value) const;
		YonaiAPI void Set(std::string locationName, glm::vec3 value) const;
		YonaiAPI void Set(std::string locationName, glm::vec4 value) const;
		YonaiAPI void Set(std::string locationName, glm::mat3 value) const;
		YonaiAPI void Set(std::string locationName, glm::mat4 value) const;

		/// <returns>Information about the uniform at location, or an invalid struct if outside of bounds</returns>
		YonaiAPI ShaderUniform GetUniformInfo(int location) const;

		/// <returns>Information about the uniform at locationName, or an invalid struct if not found</returns>
		YonaiAPI ShaderUniform GetUniformInfo(std::string locationName) const;
	};
}
