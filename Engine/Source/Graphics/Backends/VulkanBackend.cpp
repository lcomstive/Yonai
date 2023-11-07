#include <spdlog/spdlog.h>
#include <Yonai/Graphics/Backends/VulkanBackend.hpp>

using namespace Yonai::Graphics::Backends;

VulkanBackend::VulkanBackend()
{
	spdlog::debug("Initialising Vulkan backend");
}

VulkanBackend::~VulkanBackend()
{
	spdlog::debug("Destroying Vulkan backend");
}