#pragma once
#include <memory>
#include <Yonai/API.hpp>
#include <Yonai/Graphics/Backends/GraphicsBackend.hpp>

namespace Yonai::Graphics
{
	enum class GraphicsAPI : int
	{
		None = 0,
		Vulkan,
		// DirectX
	};

	struct Graphics
	{
		static std::unique_ptr<Backends::GraphicsBackend> s_Backend;

		static void Init();
		static void Destroy();

		/// <summary>
		/// Get the current graphics API
		/// </summary>
		YonaiAPI static GraphicsAPI API();

		/// <summary>
		/// Sets the graphics API
		/// </summary>
		static void API(GraphicsAPI api);

	private:
		static GraphicsAPI s_API;
	};
}