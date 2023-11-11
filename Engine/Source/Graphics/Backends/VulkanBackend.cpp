#include <Yonai/API.hpp>

#define GLFW_INCLUDE_VULKAN

#if defined(YONAI_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#if defined(YONAI_PLATFORM_APPLE)
#if defined(YONAI_PLATFORM_MAC)
#define GLFW_EXPOSE_NATIVE_COCOA
#define VK_USE_PLATFORM_MACOS_MVK
#elif defined(YONAI_PLATFORM_iOS)
#define VK_USE_PLATFORM_IOS_MVK
#endif
#endif
#if defined(YONAI_PLATFORM_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define VK_USE_PLATFORM_XCB_KHR
#endif
#if defined(YONAI_PLATFORM_ANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <glfw/glfw3native.h>
#include <vulkan/vk_enum_string_helper.h>
#include <Yonai/Graphics/Backends/Vulkan/VulkanBackend.hpp>

#include <Yonai/IO/Files.hpp>
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Scripting;
using namespace Yonai::Graphics::Backends;

const int MaxFramesInFlight = 2;
const bool EnableValidationLayers =
#ifndef NDEBUG
true;
#else
false;
#endif
const vector<const char*> ValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

const vector<const char*> DeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

#pragma region Forward Declarations
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

vector<const char*> GetRequiredExtensions();
VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo();
bool CheckValidationLayerSupport(const vector<const char*>& validationLayers);

// Queue family
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice, VkSurfaceKHR);

// Swapchain
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
VkPresentModeKHR ChooseSwapchainPresentMode(const vector<VkPresentModeKHR>& presentModes);
VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const vector<VkSurfaceFormatKHR>& formats);
SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

// Debug
VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);
#pragma endregion

void LogCriticalError(const char* msg, VkResult result)
{
	spdlog::critical("[Vulkan] {} - {}", msg, string_VkResult(result));
	exit((int)result);
}

void VulkanBackend::Init()
{
	return;

	spdlog::debug("Initialising Vulkan backend");

	// CreateInstance();
	// GetAvailableExtensions();
	// SetupDebugMessenger();

	// CreateSurface();

	// FindPhysicalDevices();
	// if (AvailablePhysicalDevices.empty())
	// 	return;
	// SelectDevice(AvailablePhysicalDevices[0]);

	// CreateSwapchain();
	CreateGraphicsPipeline();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
}

void VulkanBackend::Destroy()
{
	return;

	spdlog::debug("Destroying Vulkan backend");

	vkDeviceWaitIdle(Device);
	Cleanup();
}

void VulkanBackend::OnResized() {} // { RecreateSwapchain(); }

void VulkanBackend::Cleanup()
{
	CleanupSwapchain();

	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(Device, InFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(Device, CommandPool, nullptr);

	vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
	vkDestroyRenderPass(Device, RenderPass, nullptr);

	vkDestroyShaderModule(Device, VertexShader, nullptr);
	vkDestroyShaderModule(Device, FragmentShader, nullptr);

	vkDestroyDevice(Device, nullptr);

	if (EnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(Instance, m_DebugMessenger, nullptr);

	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkDestroyInstance(Instance, nullptr);
}

void VulkanBackend::Draw()
{
	static unsigned int currentFrame = 0;

	vkWaitForFences(Device, 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	unsigned int imageIndex;
	VkResult result = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LogCriticalError("Failed to acquire swapchain image", result);
		return;
	}

	vkResetFences(Device, 1, &InFlightFences[currentFrame]);

	vkResetCommandBuffer(CommandBuffers[currentFrame], 0);
	RecordCommandBuffer(CommandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkQueueSubmit(GraphicsQueue, 1, &submitInfo, InFlightFences[currentFrame]);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to submit draw command buffer", result);
		exit(-1);
		return;
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { Swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		RecreateSwapchain();
	else if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to present swapchain image", result);
		return;
	}

	currentFrame = (currentFrame + 1) % MaxFramesInFlight;
}

void VulkanBackend::CreateInstance()
{
	if (EnableValidationLayers && !CheckValidationLayerSupport(ValidationLayers))
	{
		spdlog::critical("Validation layers requested, but not all are available");
		exit(-1);
		return;
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Yonai Application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Yonai";
	appInfo.engineVersion = VK_MAKE_VERSION(YONAI_VERSION_MAJOR, YONAI_VERSION_MINOR, YONAI_VERSION_PATCH);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	vector<const char*> requiredExtensions = GetRequiredExtensions();

#if defined(YONAI_PLATFORM_APPLE)
	// MoltenVK 1.3+ requires VK_KHR_PORTABILITY_subset extension
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledExtensionCount = (unsigned int)requiredExtensions.size();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (EnableValidationLayers)
	{
		createInfo.ppEnabledLayerNames = ValidationLayers.data();
		createInfo.enabledLayerCount = (unsigned int)ValidationLayers.size();

		debugCreateInfo = CreateDebugInfo();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);
	if (result != VK_SUCCESS)
		return LogCriticalError("Failed to create instance", result);
}

void VulkanBackend::GetAvailableExtensions()
{
	unsigned int extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	spdlog::trace("Vulkan extensions available:");
	for (const auto& extension : extensions)
		spdlog::trace(" - {}", extension.extensionName);
}

bool CheckValidationLayerSupport(const vector<const char*>& validationLayers)
{
	unsigned int layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) != 0) continue;
			layerFound = true;
			spdlog::trace("Found '{}'", layerProperties.layerName);
			break;
		}

		if (!layerFound)
		{
			spdlog::trace("Failed to find '{}'", layerName);
			return false;
		}
	}

	return true;
}

vector<const char*> GetRequiredExtensions()
{
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if defined(YONAI_PLATFORM_APPLE)
	// MoltenVK 1.3+ requires VK_KHR_PORTABILITY_subset extension
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

	if (EnableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void VulkanBackend::CreateSurface()
{
#if defined(YONAI_PLATFORM_DESKTOP)
	VkResult result = glfwCreateWindowSurface(Instance, Window::GetNativeHandle(), nullptr, &Surface);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create surface", result);
		return;
	}
#else
#if defined(YONAI_PLATFORM_iOS)
	// vkCreateIOSSurfaceMVK
#elif defined(YONAI_PLATFORM_ANDROID)
	// vkCreateAndroidSurfaceKHR
#endif
#endif
}

void VulkanBackend::CreateSyncObjects()
{
	InFlightFences.resize(MaxFramesInFlight);
	ImageAvailableSemaphores.resize(MaxFramesInFlight);
	RenderFinishedSemaphores.resize(MaxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create in signaled state

	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Device, &fenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
		{
			spdlog::critical("Failed to create sync objects");
			return;
		}
	}
}

#pragma region Vertex Buffers
VkVertexInputBindingDescription VulkanBackend::GetBindingDescription()
{
	VkVertexInputBindingDescription description = {};
	description.binding = 0;
	description.stride = sizeof(Mesh::Vertex);
	description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return description;
}

array<VkVertexInputAttributeDescription, 3> VulkanBackend::GetAttributeDescriptions()
{
	array<VkVertexInputAttributeDescription, 3> attributes;

	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof(Mesh::Vertex, Position);

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof(Mesh::Vertex, Normal);

	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[2].offset = offsetof(Mesh::Vertex, TexCoords);

	return attributes;
}
#pragma endregion

#pragma region Devices
bool VulkanBackend::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	unsigned int extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	set<string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

int VulkanBackend::RateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	score += deviceProperties.limits.maxImageDimension2D;

	QueueFamilyIndices indices = FindQueueFamilies(device, Surface);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(device, Surface);
		swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
	}

	if (!indices.IsComplete() || !extensionsSupported || !swapChainAdequate)
		return -1; // Does not have desired features

	return score;
}

void VulkanBackend::FindPhysicalDevices()
{
	AvailablePhysicalDevices.clear();

	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		spdlog::critical("No Vulkan-compatible graphics devices were found");
		return exit(-1);
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices)
	{
		int score = RateDeviceSuitability(device);
		if (score > 0)
			candidates.insert(make_pair(score, device));
	}

	if (!candidates.empty() && candidates.rbegin()->first <= 0)
	{
		spdlog::critical("No suitable graphics devices were found");
		return exit(-1);
	}

	for (const auto& pair : candidates)
		AvailablePhysicalDevices.push_back(pair.second);
}

void VulkanBackend::SelectDevice(VkPhysicalDevice device)
{
	if (Device != VK_NULL_HANDLE)
		// TODO: Test this
		vkDestroyDevice(Device, nullptr);

	PhysicalDevice = device;
	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice, Surface);

	vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
	set<unsigned int> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

	const float QueuePriority = 1.0f;

	for (unsigned int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &QueuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Request features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Create logical device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (unsigned int)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = (unsigned int)DeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();

	if (EnableValidationLayers)
	{
		createInfo.ppEnabledLayerNames = ValidationLayers.data();
		createInfo.enabledLayerCount = (unsigned int)ValidationLayers.size();
	}
	else
		createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create logical device", result);
		return;
	}

	vkGetDeviceQueue(Device, indices.PresentFamily.value(), 0, &PresentQueue);
	vkGetDeviceQueue(Device, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
}
#pragma endregion

#pragma region Swapchain
SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapchainSupportDetails details;

	// Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

	// Formats
	unsigned int formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
	}

	// Present modes
	unsigned int presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const vector<VkSurfaceFormatKHR>& formats)
{
	for (const auto& format : formats)
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;

	// No suitable found, return first one
	return formats[0];
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// If extent is a valid value, return that
	if (capabilities.currentExtent.width != numeric_limits<unsigned int>().max())
		return capabilities.currentExtent;

	// Generate extent dimensions ourselves
	int width, height;

#if defined(YONAI_PLATFORM_DESKTOP)
	glfwGetFramebufferSize(Window::GetNativeHandle(), &width, &height);
#endif

	VkExtent2D extent = { (unsigned int)width, (unsigned int)height };
	extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return extent;
}

VkPresentModeKHR ChooseSwapchainPresentMode(const vector<VkPresentModeKHR>& presentModes)
{
	for (const auto& presentMode : presentModes)
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentMode;

	// Default to basic FIFO
	return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanBackend::CreateSwapchain()
{
	SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(PhysicalDevice, Surface);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupport.Formats);
	VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupport.PresentModes);
	VkExtent2D extent = ChooseSwapExtent(swapchainSupport.Capabilities);

	unsigned int imageCount = swapchainSupport.Capabilities.minImageCount + 1;
	if (swapchainSupport.Capabilities.maxImageCount > 0)
		imageCount = std::min(imageCount, swapchainSupport.Capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;

	// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = Direct to framebuffer
	// VK_IMAGE_USAGE_TRANSFER_DST_BIT = Offscreen rendering, then blit to framebuffer
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice, Surface);
	unsigned int queueFamilyIndices[] =
	{
		indices.GraphicsFamily.value(),
		indices.PresentFamily.value()
	};

	if (indices.GraphicsFamily != indices.PresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = swapchainSupport.Capabilities.currentTransform;

	// Ensure window is opaque
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; // Discard pixels obscured by other windows

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(Device, &createInfo, nullptr, &Swapchain);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create swapchain", result);
		return;
	}

	vkGetSwapchainImagesKHR(Device, Swapchain, &imageCount, nullptr);
	SwapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(Device, Swapchain, &imageCount, SwapchainImages.data());

	SwapchainImageFormat = surfaceFormat.format;
	SwapchainExtent = extent;

	CreateImageViews();
	CreateRenderPass();
	CreateFramebuffers();
}

void VulkanBackend::RecreateSwapchain()
{
#if defined(YONAI_PLATFORM_DESKTOP)
	// Handle minification
	int width = 0, height = 0;
	glfwGetFramebufferSize(Window::GetNativeHandle(), &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(Window::GetNativeHandle(), &width, &height);
		glfwWaitEvents();
	}
#endif

	vkDeviceWaitIdle(Device);
	CleanupSwapchain();
	CreateSwapchain();
}

void VulkanBackend::CleanupSwapchain()
{
	for (size_t i = 0; i < SwapchainFramebuffers.size(); i++)
		vkDestroyFramebuffer(Device, SwapchainFramebuffers[i], nullptr);

	for (size_t i = 0; i < SwapchainImageViews.size(); i++)
		vkDestroyImageView(Device, SwapchainImageViews[i], nullptr);

	vkDestroySwapchainKHR(Device, Swapchain, nullptr);
}

void VulkanBackend::CreateImageViews()
{
	SwapchainImageViews.resize(SwapchainImages.size());

	for (size_t i = 0; i < SwapchainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapchainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(Device, &createInfo, nullptr, &SwapchainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			LogCriticalError("Failed to create image view", result);
			return;
		}
	}
}

void VulkanBackend::CreateFramebuffers()
{
	SwapchainFramebuffers.resize(SwapchainImageViews.size());

	for (size_t i = 0; i < SwapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = { SwapchainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = SwapchainExtent.width;
		framebufferInfo.height = SwapchainExtent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(Device, &framebufferInfo, nullptr, &SwapchainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			LogCriticalError("Failed to create framebuffer", result);
			return;
		}
	}
}
#pragma endregion

#pragma region Queue Families
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	unsigned int queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.GraphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
			indices.PresentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}
#pragma endregion

#pragma region Graphics Pipeline
void VulkanBackend::CreateGraphicsPipeline()
{
	Class* graphicsClass = new Class(ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai.Graphics", "Graphics"), nullptr);
	graphicsClass->Invoke("_CreateVertexShader");
	graphicsClass->Invoke("_CreateFragmentShader");

	spdlog::debug("Created shaders");

	VkPipelineShaderStageCreateInfo vertStageInfo = {};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = VertexShader;
	vertStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragStageInfo = {};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = FragmentShader;
	fragStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	auto bindingDescription = GetBindingDescription();
	auto attributeDescriptions = GetAttributeDescriptions();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = (unsigned int)attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = viewport.y = 0;
	viewport.width = (float)SwapchainExtent.width;
	viewport.height = (float)SwapchainExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapchainExtent;

	vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStates.data();
	dynamicState.dynamicStateCount = (unsigned int)dynamicStates.size();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo  rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE; // Disable
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colourBlendAttachment = {};
	colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colourBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colourBlending = {};
	colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colourBlending.logicOpEnable = VK_FALSE;
	colourBlending.attachmentCount = 1;
	colourBlending.pAttachments = &colourBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	VkResult result = vkCreatePipelineLayout(Device, &pipelineLayoutInfo, nullptr, &PipelineLayout);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create pipeline layout", result);
		return;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colourBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = PipelineLayout;
	pipelineInfo.renderPass = RenderPass;
	pipelineInfo.subpass = 0;

	result = vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create graphics pipeline", result);
		return;
	}
}

void VulkanBackend::CreateRenderPass()
{
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format = SwapchainImageFormat;
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colourAttachmentRef = {};
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colourAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass);
	if(result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create render pass", result);
		return;
	}
}

VkShaderModule VulkanBackend::CreateShaderModule(const vector<unsigned char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = (const unsigned int*)code.data();

	VkShaderModule shader;
	VkResult result = vkCreateShaderModule(Device, &createInfo, nullptr, &shader);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create shader", result);
		return VK_NULL_HANDLE;
	}

	return shader;
}

#include <Yonai/Graphics/Graphics.hpp>
ADD_MANAGED_METHOD(Graphics, UploadVertexShader, void, (MonoArray* data), Yonai.Graphics)
{
	vector<unsigned char> code;
	code.resize(mono_array_length(data));
	for (size_t i = 0; i < code.size(); i++)
		code[i] = mono_array_get(data, char, i);

	VulkanBackend* backend = (VulkanBackend*)Graphics::Graphics::s_Backend.get();
	backend->VertexShader = backend->CreateShaderModule(code);
}

ADD_MANAGED_METHOD(Graphics, UploadFragmentShader, void, (MonoArray* data), Yonai.Graphics)
{
	vector<unsigned char> code;
	code.resize(mono_array_length(data));
	for (size_t i = 0; i < code.size(); i++)
		code[i] = mono_array_get(data, char, i);

	VulkanBackend* backend = (VulkanBackend*)Graphics::Graphics::s_Backend.get();
	backend->FragmentShader = backend->CreateShaderModule(code);
}
#pragma endregion

#pragma region Commands
void VulkanBackend::CreateCommandPool()
{
	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice, Surface);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();

	VkResult result = vkCreateCommandPool(Device, &poolInfo, nullptr, &CommandPool);
	if (result != VK_SUCCESS)
		LogCriticalError("Failed to create command pool", result);
}

void VulkanBackend::CreateCommandBuffer()
{
	CommandBuffers.resize(MaxFramesInFlight);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (unsigned int)CommandBuffers.size();

	VkResult result = vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers.data());
	if (result != VK_SUCCESS)
		LogCriticalError("Failed to create command buffer", result);
}

void VulkanBackend::RecordCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to begin recording command buffer", result);
		return;
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPass;
	renderPassInfo.framebuffer = SwapchainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapchainExtent;

	VkClearValue clearColour = { {{ 0, 0, 0, 1 }} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColour;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

	VkViewport viewport = {};
	viewport.x = viewport.y = 0;
	viewport.width = (float)SwapchainExtent.width;
	viewport.height = (float)SwapchainExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapchainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
		LogCriticalError("Failed to finish recording command buffer", result);
}
#pragma endregion

#pragma region Debug Info
VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	return createInfo;
}

void VulkanBackend::SetupDebugMessenger()
{
	if (!EnableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateDebugInfo();
	VkResult result = CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &m_DebugMessenger);
	if (result != VK_SUCCESS)
		return LogCriticalError("Failed to create debug messenger", result);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	const char* messageTypeStr = "";
	switch (messageType)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:	  messageTypeStr = "General";	  break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:  messageTypeStr = "Validation";  break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: messageTypeStr = "Performance"; break;
	}

	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	spdlog::trace("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		spdlog::info("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	spdlog::warn("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		spdlog::error("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	}

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(instance, debugMessenger, pAllocator);
}
#pragma endregion

ADD_MANAGED_METHOD(VulkanInstance, Create, void*, (
	MonoString* monoAppName,
	int appMajor,
	int appMinor,
	int appPatch
	), Yonai.Graphics.Backends.Vulkan)
{
	char* appName = mono_string_to_utf8(monoAppName);

	if (EnableValidationLayers && !CheckValidationLayerSupport(ValidationLayers))
	{
		spdlog::critical("Validation layers requested, but not all are available");
		exit(-1);
		return nullptr;
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(appMajor, appMinor, appPatch);
	appInfo.pEngineName = "Yonai";
	appInfo.engineVersion = VK_MAKE_VERSION(YONAI_VERSION_MAJOR, YONAI_VERSION_MINOR, YONAI_VERSION_PATCH);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	vector<const char*> requiredExtensions = GetRequiredExtensions();

#if defined(YONAI_PLATFORM_APPLE)
	// MoltenVK 1.3+ requires VK_KHR_PORTABILITY_subset extension
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledExtensionCount = (unsigned int)requiredExtensions.size();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (EnableValidationLayers)
	{
		createInfo.ppEnabledLayerNames = ValidationLayers.data();
		createInfo.enabledLayerCount = (unsigned int)ValidationLayers.size();

		debugCreateInfo = CreateDebugInfo();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	mono_free(appName);

	VkInstance instance;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create instance", result);
		return nullptr;
	}

	return instance;
}

ADD_MANAGED_METHOD(VulkanInstance, Destroy, void, (void* instance), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyInstance((VkInstance)instance, nullptr); }

ADD_MANAGED_METHOD(VulkanGraphicsBackend, GetAvailableExtensions, MonoArray*, (), Yonai.Graphics.Backends.Vulkan)
{
	unsigned int extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_string_class(), extensionCount);
	for(unsigned int i = 0; i < extensionCount; i++)
	{
		MonoString* monoName = mono_string_new(mono_domain_get(), extensions[i].extensionName);
		mono_array_set(output, MonoString*, i, monoName);
	}
	return output;
}

ADD_MANAGED_METHOD(VulkanInstance, CreateDebugMessenger, void*, (void* instance), Yonai.Graphics.Backends.Vulkan)
{
	if (!EnableValidationLayers) return nullptr;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateDebugInfo();
	VkDebugUtilsMessengerEXT messenger;
	VkResult result = CreateDebugUtilsMessengerEXT((VkInstance)instance, &createInfo, nullptr, &messenger);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create debug messenger", result);
		return nullptr;
	}
	return messenger;
}

ADD_MANAGED_METHOD(VulkanInstance, DestroyDebugMessenger, void, (void* instance, void* messenger), Yonai.Graphics.Backends.Vulkan)
{
	if (EnableValidationLayers && messenger)
		DestroyDebugUtilsMessengerEXT((VkInstance)instance, (VkDebugUtilsMessengerEXT)messenger, nullptr);
}

ADD_MANAGED_METHOD(VulkanInstance, CreateSurface, void*, (void* instance), Yonai.Graphics.Backends.Vulkan)
{
	VkSurfaceKHR surface;
#if defined(YONAI_PLATFORM_DESKTOP)
	VkResult result = glfwCreateWindowSurface((VkInstance)instance, Window::GetNativeHandle(), nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create surface", result);
		return nullptr;
	}
#else
#if defined(YONAI_PLATFORM_iOS)
	// vkCreateIOSSurfaceMVK
#elif defined(YONAI_PLATFORM_ANDROID)
	// vkCreateAndroidSurfaceKHR
#endif
#endif

	return surface;
}

ADD_MANAGED_METHOD(VulkanInstance, DestroySurface, void, (void* instance, void* surface), Yonai.Graphics.Backends.Vulkan)
{ vkDestroySurfaceKHR((VkInstance)instance, (VkSurfaceKHR)surface, nullptr); }

ADD_MANAGED_METHOD(VulkanInstance, GetPhysicalDevices, MonoArray*, (void* instanceRaw), Yonai.Graphics.Backends.Vulkan)
{
	VkInstance instance = (VkInstance)instanceRaw;

	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		spdlog::critical("No Vulkan-compatible graphics devices were found");
		return nullptr;
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_intptr_class(), deviceCount);

	for (unsigned int i = 0; i < deviceCount; i++)
		mono_array_set(output, VkPhysicalDevice, i, devices[i]);

	return output;
}

ADD_MANAGED_METHOD(VulkanDevice, GetQueueFamilyProperties, MonoArray*, (void* physicalDevice), Yonai.Graphics.Backends.Vulkan)
{
	VkPhysicalDevice device = (VkPhysicalDevice)physicalDevice;
	unsigned int queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_int32_class(), queueFamilyCount);
	for (unsigned int i = 0; i < queueFamilyCount; i++)
		mono_array_set(output, int, i, (int)queueFamilies[i].queueFlags);

	return output;
}

ADD_MANAGED_METHOD(VulkanDevice, CreateDevice, void*, (void* physicalDevice, unsigned int graphicsFamily, unsigned int presentFamily), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
	set<unsigned int> uniqueQueueFamilies = { graphicsFamily, presentFamily };

	const float QueuePriority = 1.0f;

	for (unsigned int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &QueuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Request features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Create logical device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (unsigned int)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = (unsigned int)DeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();

	if (EnableValidationLayers)
	{
		createInfo.ppEnabledLayerNames = ValidationLayers.data();
		createInfo.enabledLayerCount = (unsigned int)ValidationLayers.size();
	}
	else
		createInfo.enabledLayerCount = 0;

	VkDevice device;
	VkResult result = vkCreateDevice((VkPhysicalDevice)physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create logical device", result);
		return nullptr;
	}

	return device;
}

ADD_MANAGED_METHOD(VulkanDevice, GetDeviceQueue, void*, (void* device, unsigned int queueFamilyIndex, unsigned int queueIndex), Yonai.Graphics.Backends.Vulkan)
{
	VkQueue queue;
	vkGetDeviceQueue((VkDevice)device, queueFamilyIndex, queueIndex, &queue);
	return queue;
}

ADD_MANAGED_METHOD(VulkanDevice, GetPhysicalDeviceSurfaceSupport, bool, (void* device, unsigned int queueFamilyIndex, void* surface), Yonai.Graphics.Backends.Vulkan)
{
	VkBool32 presentSupport;
	vkGetPhysicalDeviceSurfaceSupportKHR(
		(VkPhysicalDevice)device,
		queueFamilyIndex,
		(VkSurfaceKHR)surface,
		&presentSupport
	);
	return presentSupport;
}

ADD_MANAGED_METHOD(VulkanDevice, GetPhysicalDeviceProperties, MonoString*, (void* device, unsigned int* outID, unsigned int* outDriverVersion, int* outDeviceType), Yonai.Graphics.Backends.Vulkan)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties((VkPhysicalDevice)device, &properties);

	*outID = properties.deviceID;
	*outDriverVersion = properties.driverVersion;
	*outDeviceType = (int)properties.deviceType;
	return mono_string_new(mono_domain_get(), properties.deviceName);
}

ADD_MANAGED_METHOD(VulkanSwapchain, Create, void*, (
	void* inPhysicalDevice,
	void* inDevice,
	void* inSurface,
	void* oldSwapchain,
	int* outImageFormat,
	unsigned int* outExtentsWidth,
	unsigned int* outExtentsHeight
), Yonai.Graphics.Backends.Vulkan)
{
	VkDevice device = (VkDevice)inDevice;
	VkSurfaceKHR surface = (VkSurfaceKHR)inSurface;
	VkPhysicalDevice physicalDevice = (VkPhysicalDevice)inPhysicalDevice;

	SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupport.Formats);
	VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupport.PresentModes);
	VkExtent2D extent = ChooseSwapExtent(swapchainSupport.Capabilities);

	unsigned int imageCount = swapchainSupport.Capabilities.minImageCount + 1;
	if (swapchainSupport.Capabilities.maxImageCount > 0)
		imageCount = std::min(imageCount, swapchainSupport.Capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;

	// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = Direct to framebuffer
	// VK_IMAGE_USAGE_TRANSFER_DST_BIT = Offscreen rendering, then blit to framebuffer
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
	unsigned int queueFamilyIndices[] =
	{
		indices.GraphicsFamily.value(),
		indices.PresentFamily.value()
	};

	if (indices.GraphicsFamily != indices.PresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = swapchainSupport.Capabilities.currentTransform;

	// Ensure window is opaque
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; // Discard pixels obscured by other windows

	createInfo.oldSwapchain = oldSwapchain ? (VkSwapchainKHR)oldSwapchain : VK_NULL_HANDLE;

	VkSwapchainKHR swapchain;
	VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create swapchain", result);
		return nullptr;
	}

	*outImageFormat = surfaceFormat.format;
	*outExtentsWidth = extent.width;
	*outExtentsHeight = extent.height;

	return swapchain;
}

ADD_MANAGED_METHOD(VulkanSwapchain, Destroy, void, (void* device, void* swapchain), Yonai.Graphics.Backends.Vulkan)
{ vkDestroySwapchainKHR((VkDevice)device, (VkSwapchainKHR)swapchain, nullptr); }

ADD_MANAGED_METHOD(VulkanSwapchain, GetImages, MonoArray*, (void* inDevice, void* inSwapchain), Yonai.Graphics.Backends.Vulkan)
{
	VkDevice device = (VkDevice)inDevice;
	VkSwapchainKHR swapchain = (VkSwapchainKHR)inSwapchain;

	unsigned int imageCount = 0;
	vector<VkImage> swapchainImages;
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_intptr_class(), imageCount);
	for (unsigned int i = 0; i < imageCount; i++)
		mono_array_set(output, void*, i, swapchainImages[i]);

	return output;
}

ADD_MANAGED_METHOD(VulkanImage, CreateImageView, void*, (void* image, void* inDevice, int imageFormat), Yonai.Graphics.Backends.Vulkan)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = (VkImage)image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = (VkFormat)imageFormat;

	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	VkResult result = vkCreateImageView((VkDevice)inDevice, &createInfo, nullptr, &imageView);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create image view", result);
		return nullptr;
	}

	return imageView;
}

ADD_MANAGED_METHOD(VulkanRenderPass, Create, void*,
	(void* device, int attachmentCount, void* attachments,
		int subpassCount, void* subpasses, int dependencyCount, void* dependencies),
	Yonai.Graphics.Backends.Vulkan)
{
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (unsigned int)attachmentCount;
	renderPassInfo.pAttachments = (VkAttachmentDescription*)attachments;
	renderPassInfo.subpassCount = (unsigned int)subpassCount;
	renderPassInfo.pSubpasses = (VkSubpassDescription*)subpasses;
	renderPassInfo.dependencyCount = dependencyCount;
	renderPassInfo.pDependencies = (VkSubpassDependency*)dependencies;

	VkRenderPass renderPass;
	VkResult result = vkCreateRenderPass((VkDevice)device, &renderPassInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create render pass", result);
		return nullptr;
	}
	return renderPass;
}

ADD_MANAGED_METHOD(VulkanRenderPass, Destroy, void, (void* device, void* renderPass), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyRenderPass((VkDevice)device, (VkRenderPass)renderPass, nullptr); }

ADD_MANAGED_METHOD(VulkanFramebuffer, Create, void*,
	(void* device, void* renderPass, MonoArray* monoAttachments,
		unsigned int width, unsigned int height),
	Yonai.Graphics.Backends.Vulkan)
{
	unsigned int attachmentCount = (unsigned int)mono_array_length(monoAttachments);
	vector<VkImageView> attachments(attachmentCount);
	for (size_t i = 0; i < attachmentCount; i++)
		attachments[i] = mono_array_get(monoAttachments, VkImageView, i);

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = (VkRenderPass)renderPass;
	framebufferInfo.attachmentCount = (unsigned int)attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width  = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	VkFramebuffer output;
	VkResult result = vkCreateFramebuffer((VkDevice)device, &framebufferInfo, nullptr, &output);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create framebuffer", result);
		return nullptr;
	}
	return output;
}

ADD_MANAGED_METHOD(VulkanFramebuffer, Destroy, void, (void* device, void* framebuffer), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyFramebuffer((VkDevice)device, (VkFramebuffer)framebuffer, nullptr); }