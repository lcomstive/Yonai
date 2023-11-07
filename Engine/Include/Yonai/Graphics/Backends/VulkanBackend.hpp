#pragma once
#include <Yonai/Graphics/Backends/GraphicsBackend.hpp>

namespace Yonai::Graphics::Backends
{
	struct VulkanBackend : public GraphicsBackend
	{
		VulkanBackend();
		~VulkanBackend();
	};
}