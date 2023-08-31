#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <Yonai/ResourceID.hpp>

namespace Yonai::Graphics
{
	class Mesh
	{
	public:
		enum class DrawMode : unsigned char
		{
			Points			= GL_POINTS,
			Lines			= GL_LINES,
			LineStrip		= GL_LINE_STRIP,
			Triangles		= GL_TRIANGLES,
			TriangleStrip	= GL_TRIANGLE_STRIP,
			TriangleFan		= GL_TRIANGLE_FAN,
			Quads			= GL_QUADS,
			QuadStrip		= GL_QUAD_STRIP
		};

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoords;
		};

	private:
		unsigned int m_VBO, m_VAO, m_EBO;

		DrawMode m_DrawMode;
		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;

		void Setup();

	public:
		Mesh();
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, DrawMode drawMode = DrawMode::Triangles);
		~Mesh();

		YonaiAPI void Draw();
		YonaiAPI void Import(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, DrawMode drawMode = DrawMode::Triangles);

		YonaiAPI std::vector<Vertex>& GetVertices();
		YonaiAPI void SetVertices(std::vector<Vertex>& vertices);
		YonaiAPI std::vector<unsigned int>& GetIndices();
		YonaiAPI void SetIndices(std::vector<unsigned int>& indices);

		YonaiAPI static ResourceID Quad();
		YonaiAPI static ResourceID Cube();
		YonaiAPI static ResourceID Sphere();
	};
}