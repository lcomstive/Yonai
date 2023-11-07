#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Yonai/ResourceID.hpp>

namespace Yonai::Graphics
{
	class Mesh
	{
	public:
		enum class DrawMode : unsigned char
		{
			Points,
			Lines,
			LineStrip,
			Triangles,
			TriangleStrip,
			TriangleFan,
			Quads,
			QuadStrip
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