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
		VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkFormat SwapchainImageFormat;
		VkExtent2D SwapchainExtent;

		// Pipeline //
		VkPipeline GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;

		// Commands //
		VkCommandPool CommandPool = VK_NULL_HANDLE;;
		std::vector<VkCommandBuffer> CommandBuffers = {};

		VkRenderPass RenderPass = VK_NULL_HANDLE;

		std::vector<VkImage> SwapchainImages = {};
		std::vector<VkImageView> SwapchainImageViews = {};
		std::vector<VkFramebuffer> SwapchainFramebuffers = {};
		std::vector<VkPhysicalDevice> AvailablePhysicalDevices = {};

		YonaiAPI void Init() override;
		YonaiAPI void Draw() override;
		YonaiAPI void Destroy() override;
		YonaiAPI void OnResized() override;

		YonaiAPI void SelectDevice(VkPhysicalDevice device);

		VkShaderModule VertexShader = VK_NULL_HANDLE;
		VkShaderModule FragmentShader = VK_NULL_HANDLE;
		YonaiAPI VkShaderModule CreateShaderModule(const std::vector<unsigned char>& code);

	private:
		std::vector<VkFence> InFlightFences;
		std::vector<VkSemaphore> ImageAvailableSemaphores;
		std::vector<VkSemaphore> RenderFinishedSemaphores;

		VkDebugUtilsMessengerEXT m_DebugMessenger;

		static const bool EnableValidationLayers;
		static const std::vector<const char*> DeviceExtensions;
		static const std::vector<const char*> ValidationLayers;

		struct QueueFamilyIndices
		{
			std::optional<unsigned int> GraphicsFamily;
			std::optional<unsigned int> PresentFamily;

			bool IsComplete() { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		};

		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities = {};
			std::vector<VkSurfaceFormatKHR> Formats = {};
			std::vector<VkPresentModeKHR> PresentModes = {};
		};

		void Cleanup();
		void CreateSurface();
		void CreateInstance();
		void CreateSyncObjects();
		void FindPhysicalDevices();
		void SetupDebugMessenger();
		void GetAvailableExtensions();
		std::vector<const char*> GetRequiredExtensions();
		int RateDeviceSuitability(VkPhysicalDevice device);
		VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool CheckValidationLayerSupport(const std::vector<const char*>& layers);
		void CreateRenderPass();

#pragma region Commands
		void CreateCommandPool();
		void CreateCommandBuffer();
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex);
#pragma endregion

#pragma region Swapchain
		void CreateSwapchain();
		void CleanupSwapchain();
		void CreateImageViews();
		void RecreateSwapchain();
		void CreateFramebuffers();

		SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
		VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
#pragma endregion

#pragma region Graphics Pipeline
		void CreateGraphicsPipeline();
#pragma endregion
	};
}