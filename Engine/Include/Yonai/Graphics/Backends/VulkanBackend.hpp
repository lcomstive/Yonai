#pragma once
#include <vector>
#include <optional>
#include <Yonai/API.hpp>
#include <vulkan/vulkan.h>
#include <Yonai/Graphics/Backends/GraphicsBackend.hpp>

namespace Yonai::Graphics::Backends
{
	struct VulkanBackend : public GraphicsBackend
	{
		VkDevice Device = VK_NULL_HANDLE;
		VkInstance Instance = VK_NULL_HANDLE;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		VkQueue PresentQueue = VK_NULL_HANDLE;
		VkQueue GraphicsQueue = VK_NULL_HANDLE;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		std::vector<VkPhysicalDevice> AvailablePhysicalDevices = {};

		YonaiAPI VulkanBackend();
		YonaiAPI ~VulkanBackend() override;

		YonaiAPI void SelectDevice(VkPhysicalDevice device);

	private:
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		static const bool EnableValidationLayers;
		static const std::vector<const char*> ValidationLayers;

		struct QueueFamilyIndices
		{
			std::optional<unsigned int> GraphicsFamily;
			std::optional<unsigned int> PresentFamily;

			bool IsComplete() { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		};

		void Cleanup();
		void CreateSurface();
		void CreateInstance();
		void FindPhysicalDevices();
		void SetupDebugMessenger();
		void GetAvailableExtensions();
		std::vector<const char*> GetRequiredExtensions();
		int RateDeviceSuitability(VkPhysicalDevice device);
		VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool CheckValidationLayerSupport(const std::vector<const char*>& layers);
	};
}