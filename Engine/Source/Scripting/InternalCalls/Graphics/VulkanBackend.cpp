#include <Yonai/API.hpp>

#define VMA_IMPLEMENTATION
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
#include <optional>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h> // VMA
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <GLFW/glfw3native.h>

#include <Yonai/IO/Files.hpp>
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Scripting;

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
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,

#if defined(YONAI_PLATFORM_APPLE)
	"VK_KHR_portability_subset"
#endif
};

PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR_;
PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR_;

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

// Queue family
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
// Swapchain
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// If extent is a valid value, return that
	if (capabilities.currentExtent.width != numeric_limits<unsigned int>().max())
		return capabilities.currentExtent;

	// Generate extent dimensions ourselves
	int width, height;

#if defined(YONAI_PLATFORM_DESKTOP)
	glfwGetFramebufferSize(Yonai::Window::GetNativeHandle(), &width, &height);
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

VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const vector<VkSurfaceFormatKHR>& formats)
{
	for (const auto& format : formats)
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;

	// No suitable found, return first one
	return formats[0];
}

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

// Debug
VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);
#pragma endregion

void LogCriticalError(const char* msg, VkResult result)
{
	spdlog::critical("[Vulkan] {} - {}", msg, (int)result);
	exit((int)result);
}

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
	default:
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	spdlog::trace("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		spdlog::trace("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
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
	requiredExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
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
	VkResult result = glfwCreateWindowSurface((VkInstance)instance, Yonai::Window::GetNativeHandle(), nullptr, &surface);
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
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features sync2Features = {};
	sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	sync2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	sync2Features.pNext = &dynamicRenderingFeatures;

	// Create logical device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (unsigned int)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.pNext = &sync2Features;

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

	vkCmdBeginRenderingKHR_ = (PFN_vkCmdBeginRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR");
	vkCmdEndRenderingKHR_ = (PFN_vkCmdEndRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR");

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

ADD_MANAGED_METHOD(VulkanDevice, WaitIdle, void, (void* device), Yonai.Graphics.Backends.Vulkan)
{ vkDeviceWaitIdle((VkDevice)device); }

ADD_MANAGED_METHOD(VulkanDevice, CreateAllocator, void*, (VkPhysicalDevice physical, VkDevice logical, VkInstance instance), Yonai.Graphics.Backends.Vulkan)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = physical;
	allocatorInfo.device = logical;
	allocatorInfo.instance = instance;

	VmaAllocator allocator;
	vmaCreateAllocator(&allocatorInfo, &allocator);
	return allocator;
}

ADD_MANAGED_METHOD(VulkanDevice, DestroyAllocator, void, (void* allocator), Yonai.Graphics.Backends.Vulkan)
{ vmaDestroyAllocator((VmaAllocator)allocator); }

ADD_MANAGED_METHOD(VulkanDevice, GetPhysicalDeviceLimits, void, (void* physicalDevice, VkPhysicalDeviceLimits* output), Yonai.Graphics.Backends.Vulkan)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties((VkPhysicalDevice)physicalDevice, &properties);
	*output = properties.limits;
}

ADD_MANAGED_METHOD(VulkanDescriptorSetLayout, Create, int, (void* device, void* inLayoutBindings, unsigned int layoutBindingCount, void** output), Yonai.Graphics.Backends.Vulkan)
{
	VkDescriptorSetLayoutBinding* layoutBindings = (VkDescriptorSetLayoutBinding*)inLayoutBindings;
	for (unsigned int i = 0; i < layoutBindingCount; i++)
		layoutBindings[i].pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = layoutBindingCount;
	info.pBindings = layoutBindings;

	VkDescriptorSetLayout layout;
	VkResult result = vkCreateDescriptorSetLayout((VkDevice)device, &info, nullptr, &layout);
	if(result == VK_SUCCESS)
		*output = layout;
	return result;
}

ADD_MANAGED_METHOD(VulkanDescriptorSetLayout, Destroy, void, (void* device, void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyDescriptorSetLayout((VkDevice)device, (VkDescriptorSetLayout)handle, nullptr); }

ADD_MANAGED_METHOD(VulkanDescriptorPool, Create, int, (void* device, MonoArray* inPoolSizes, unsigned int maxDescriptorSets, void** output), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.resize(mono_array_length(inPoolSizes));
	for (size_t i = 0; i < poolSizes.size(); i++)
		poolSizes[i] = mono_array_get(inPoolSizes, VkDescriptorPoolSize, i);

	VkDescriptorPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.poolSizeCount = (unsigned int)poolSizes.size();
	info.pPoolSizes = poolSizes.data();
	info.maxSets = maxDescriptorSets;

	VkDescriptorPool pool;
	VkResult result = vkCreateDescriptorPool((VkDevice)device, &info, nullptr, &pool);
	if (result == VK_SUCCESS)
		*output = pool;
	return result;
}

ADD_MANAGED_METHOD(VulkanDescriptorPool, Destroy, void, (void* device, void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyDescriptorPool((VkDevice)device, (VkDescriptorPool)handle, nullptr); }

ADD_MANAGED_METHOD(VulkanDescriptorPool, Reset, void, (VkDevice device, VkDescriptorPool handle), Yonai.Graphics.Backends.Vulkan)
{ vkResetDescriptorPool(device, handle, 0); }

ADD_MANAGED_METHOD(VulkanDescriptorPool, AllocateDescriptorSets, int, (
	void* inDevice,
	void* handle,
	int count,
	void* descriptorSetLayout,
	MonoArray** output
), Yonai.Graphics.Backends.Vulkan)
{
	VkDevice device = (VkDevice)inDevice;

	vector<VkDescriptorSetLayout> layouts(count, (VkDescriptorSetLayout)descriptorSetLayout);
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = (VkDescriptorPool)handle;
	info.descriptorSetCount = count;
	info.pSetLayouts = layouts.data();

	vector<VkDescriptorSet> descriptorSets;
	descriptorSets.resize(count);
	VkResult result = vkAllocateDescriptorSets(device, &info, descriptorSets.data());
	if (result != VK_SUCCESS)
		return result;

	*output = mono_array_new(mono_domain_get(), mono_get_intptr_class(), count);
	for (int i = 0; i < count; i++)
		mono_array_set(*output, void*, i, descriptorSets[i]);

	return result;
}

ADD_MANAGED_METHOD(VulkanDescriptorSet, UpdateDescriptorSet, void, (void* device, MonoArray* inSets), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkWriteDescriptorSet> sets;
	sets.resize(mono_array_length(inSets));
	for (size_t i = 0; i < sets.size(); i++)
	{
		sets[i] = mono_array_get(inSets, VkWriteDescriptorSet, i);
		sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	}

	vkUpdateDescriptorSets((VkDevice)device, (unsigned int)sets.size(), sets.data(), 0, nullptr);
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
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

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

ADD_MANAGED_METHOD(VulkanSwapchain, AcquireNextImage, int, (void* device, void* swapchain, unsigned int timeout, void* semaphore, void* fence, unsigned int* outIndex), Yonai.Graphics.Backends.Vulkan)
{
	return (int)vkAcquireNextImageKHR((VkDevice)device, (VkSwapchainKHR)swapchain, timeout, (VkSemaphore)semaphore, (VkFence)fence, outIndex);
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

ADD_MANAGED_METHOD(VulkanShaderModule, Create, void*, (void* device, MonoArray* data), Yonai.Graphics.Backends.Vulkan)
{
	vector<unsigned char> code;
	code.resize(mono_array_length(data));
	for (size_t i = 0; i < code.size(); i++)
		code[i] = mono_array_get(data, char, i);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = (const unsigned int*)code.data();

	VkShaderModule shader;
	VkResult result = vkCreateShaderModule((VkDevice)device, &createInfo, nullptr, &shader);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create shader", result);
		return nullptr;
	}

	return shader;
}

ADD_MANAGED_METHOD(VulkanShaderModule, Destroy, void, (void* device, void* shaderModule), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyShaderModule((VkDevice)device, (VkShaderModule)shaderModule, nullptr); }

struct VkPipelineColorBlendStateCreateInfoManaged
{
	float blendConstantR;
	float blendConstantG;
	float blendConstantB;
	float blendConstantA;
	
	bool logicOpEnable;
	VkLogicOp logicOp;
	uint32_t attachmentCount;
	const VkPipelineColorBlendAttachmentState* pAttachments;
};

struct VkGraphicsPipelineCreateInfoManaged
{
	unsigned int StageCount;
	VkPipelineShaderStageCreateInfo* Stages;
	VkPipelineVertexInputStateCreateInfo InputState;
	VkPipelineInputAssemblyStateCreateInfo InputAssembly;
	
	unsigned int ViewportsCount;
	VkViewport* Viewports;

	unsigned int ScissorsCount;
	VkRect2D* Scissors;

	VkPipelineRasterizationStateCreateInfo Rasterization;
	VkPipelineMultisampleStateCreateInfo Multisample;
	VkPipelineDepthStencilStateCreateInfo* DepthStencil;
	VkPipelineColorBlendStateCreateInfoManaged ColorBlendState;

	unsigned int DynamicStatesCount;
	VkDynamicState* DynamicStates;

	VkRenderPass RenderPass;
	unsigned int Subpass;

	unsigned int DescriptorSetLayoutCount;
	VkDescriptorSetLayout* DescriptorSetLayouts;

	unsigned int PushConstantRangeCount;
	VkPushConstantRange* PushConstantRanges;

	VkPipelineRenderingCreateInfo* RenderingInfo;
};

ADD_MANAGED_METHOD(VulkanGraphicsPipeline, Create, void*, (void* inDevice, void* inPipelineInfo, void** outPipelineLayout), Yonai.Graphics.Backends.Vulkan)
{
	VkDevice device = (VkDevice)inDevice;
	VkGraphicsPipelineCreateInfoManaged* managed = (VkGraphicsPipelineCreateInfoManaged*)inPipelineInfo;

	managed->InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	managed->InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	managed->Rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	managed->Multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

	for (unsigned int i = 0; i < managed->StageCount; i++)
		managed->Stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	if (managed->DepthStencil)
		managed->DepthStencil->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	VkPipelineColorBlendStateCreateInfo blendState = {};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.attachmentCount = managed->ColorBlendState.attachmentCount;
	blendState.pAttachments = managed->ColorBlendState.pAttachments;
	blendState.logicOp = managed->ColorBlendState.logicOp;
	blendState.logicOpEnable = managed->ColorBlendState.logicOpEnable;

	blendState.blendConstants[0] = managed->ColorBlendState.blendConstantR;
	blendState.blendConstants[1] = managed->ColorBlendState.blendConstantG;
	blendState.blendConstants[2] = managed->ColorBlendState.blendConstantB;
	blendState.blendConstants[3] = managed->ColorBlendState.blendConstantA;

	VkPipelineDynamicStateCreateInfo dynamicStates = {};
	dynamicStates.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStates.pDynamicStates = managed->DynamicStates;
	dynamicStates.dynamicStateCount = managed->DynamicStatesCount;

	VkPipelineViewportStateCreateInfo viewports = {};
	viewports.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewports.pViewports = managed->Viewports;
	viewports.viewportCount = managed->ViewportsCount;
	viewports.pScissors = managed->Scissors;
	viewports.scissorCount = managed->ScissorsCount;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = managed->DescriptorSetLayoutCount;
	pipelineLayoutInfo.pSetLayouts = managed->DescriptorSetLayouts;

	pipelineLayoutInfo.pPushConstantRanges = managed->PushConstantRanges;
	pipelineLayoutInfo.pushConstantRangeCount = managed->PushConstantRangeCount;

	VkPipelineLayout pipelineLayout;
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create pipeline layout", result);
		return nullptr;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = managed->StageCount;
	pipelineInfo.pStages = managed->Stages;
	pipelineInfo.pVertexInputState = &managed->InputState;
	pipelineInfo.pInputAssemblyState = &managed->InputAssembly;
	pipelineInfo.pViewportState = &viewports;
	pipelineInfo.pRasterizationState = &managed->Rasterization;
	pipelineInfo.pMultisampleState = &managed->Multisample;
	pipelineInfo.pColorBlendState = &blendState;
	pipelineInfo.pDynamicState = &dynamicStates;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = managed->RenderPass;
	pipelineInfo.subpass = managed->Subpass;
	pipelineInfo.pDepthStencilState = managed->DepthStencil;

	if (managed->RenderingInfo)
		pipelineInfo.pNext = managed->RenderingInfo;

	VkPipeline pipeline;
	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create graphics pipeline", result);
		return nullptr;
	}
	*outPipelineLayout = pipelineLayout;
	return pipeline;
}

ADD_MANAGED_METHOD(VulkanGraphicsPipeline, Destroy, void, (void* device, void* handle, void* layout), Yonai.Graphics.Backends.Vulkan)
{
	vkDestroyPipelineLayout((VkDevice)device, (VkPipelineLayout)layout, nullptr);
	vkDestroyPipeline((VkDevice)device, (VkPipeline)handle, nullptr);
}

struct VkComputePipelineCreateInfoManaged
{
	VkPipelineCreateFlagBits Flags;
	VkShaderModule Shader;

	unsigned int DescriptorSetLayoutCount;
	VkDescriptorSetLayout* DescriptorSetLayouts;

	unsigned int PushConstantRangeCount;
	VkPushConstantRange* PushConstantRanges;

	VkPipeline BasePipeline;
};

ADD_MANAGED_METHOD(VulkanComputePipeline, Create, void*, (VkDevice device, void* inPipelineInfo, VkPipelineLayout* outPipelineLayout), Yonai.Graphics.Backends.Vulkan)
{
	VkComputePipelineCreateInfoManaged* managed = (VkComputePipelineCreateInfoManaged*)inPipelineInfo;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = managed->DescriptorSetLayoutCount;
	pipelineLayoutInfo.pSetLayouts = managed->DescriptorSetLayouts;

	pipelineLayoutInfo.pPushConstantRanges = managed->PushConstantRanges;
	pipelineLayoutInfo.pushConstantRangeCount = managed->PushConstantRangeCount;

	VkPipelineLayout pipelineLayout;
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create pipeline layout", result);
		return nullptr;
	}

	VkPipelineShaderStageCreateInfo stageInfo = {};
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageInfo.module = managed->Shader;
	stageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.stage = stageInfo;
	pipelineInfo.flags = managed->Flags;

	pipelineInfo.basePipelineIndex = 0;
	pipelineInfo.basePipelineHandle = managed->BasePipeline;

	VkPipeline pipeline;
	result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create compute pipeline", result);
		return nullptr;
	}
	*outPipelineLayout = pipelineLayout;
	return pipeline;
}

ADD_MANAGED_METHOD(VulkanComputePipeline, Destroy, void, (VkDevice device, VkPipeline handle, VkPipelineLayout layout), Yonai.Graphics.Backends.Vulkan)
{
	vkDestroyPipelineLayout(device, layout, nullptr);
	vkDestroyPipeline(device, handle, nullptr);
}

ADD_MANAGED_METHOD(VulkanCommandPool, Create, void*, (void* device, unsigned int graphicsQueueIndex, int flag), Yonai.Graphics.Backends.Vulkan)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = (VkCommandPoolCreateFlagBits)flag;
	poolInfo.queueFamilyIndex = graphicsQueueIndex;

	VkCommandPool commandPool;
	VkResult result = vkCreateCommandPool((VkDevice)device, &poolInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create command pool", result);
		return nullptr;
	}
	return commandPool;
}

ADD_MANAGED_METHOD(VulkanCommandPool, Destroy, void, (void* device, void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyCommandPool((VkDevice)device, (VkCommandPool)handle, nullptr); }

ADD_MANAGED_METHOD(VulkanCommandPool, CreateCommandBuffers, void,
	(void* device, void* commandPool, unsigned int count, int bufferLevel, MonoArray** outHandles),
	Yonai.Graphics.Backends.Vulkan)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = (VkCommandPool)commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;

	vector<VkCommandBuffer> buffers;
	buffers.resize(count);
	VkResult result = vkAllocateCommandBuffers((VkDevice)device, &allocInfo, buffers.data());
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create command buffer", result);
		outHandles = nullptr;
		return;
	}

	*outHandles = mono_array_new(mono_domain_get(), mono_get_intptr_class(), count);
	for (unsigned int i = 0; i < count; i++)
		mono_array_set(*outHandles, VkCommandBuffer, i, buffers[i]);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, Destroy, void, (void* device, void* cmdPool, void* cmdBuffer), Yonai.Graphics.Backends.Vulkan)
{ vkFreeCommandBuffers((VkDevice)device, (VkCommandPool)cmdPool, 1, (VkCommandBuffer*)&cmdBuffer); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, Begin, int, (void* handle, int usage), Yonai.Graphics.Backends.Vulkan)
{
	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = (VkCommandBufferUsageFlags)usage;
	return (int)vkBeginCommandBuffer((VkCommandBuffer)handle, &info);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, End, int, (void* handle), Yonai.Graphics.Backends.Vulkan)
{ return (int)vkEndCommandBuffer((VkCommandBuffer)handle); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, Reset, int, (void* handle, int flag), Yonai.Graphics.Backends.Vulkan)
{ return (int)vkResetCommandBuffer((VkCommandBuffer)handle, (VkCommandBufferResetFlags)flag); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, BindPipeline, void, (void* handle, int bindPoint, void* pipeline), Yonai.Graphics.Backends.Vulkan)
{ vkCmdBindPipeline((VkCommandBuffer)handle, (VkPipelineBindPoint)bindPoint, (VkPipeline)pipeline); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, SetViewport, void, (void* handle, void* viewport, int index), Yonai.Graphics.Backends.Vulkan)
{ vkCmdSetViewport((VkCommandBuffer)handle, index, 1, (VkViewport*)viewport); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, SetScissor, void, (void* handle, void* scissor, int index), Yonai.Graphics.Backends.Vulkan)
{ vkCmdSetScissor((VkCommandBuffer)handle, index, 1, (VkRect2D*)scissor); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, Draw, void,
	(void* handle, unsigned int vertexCount, unsigned int instanceCount, unsigned int firstVertex, unsigned int firstInstance),
	Yonai.Graphics.Backends.Vulkan)
{ vkCmdDraw((VkCommandBuffer)handle, vertexCount, instanceCount, firstVertex, firstInstance); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, DrawIndexed, void,
	(void* handle, unsigned int indexCount, unsigned int instanceCount,
	unsigned int firstIndex, unsigned int vertexOffset, unsigned int firstInstance),
	Yonai.Graphics.Backends.Vulkan)
{ vkCmdDrawIndexed((VkCommandBuffer)handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance); }

struct VkClearValueManaged
{
	float ColourR;
	float ColourG;
	float ColourB;
	float ColourA;
	VkClearDepthStencilValue DepthStencil;
	bool UseColour;
};

ADD_MANAGED_METHOD(VulkanCommandBuffer, BeginRenderPass, void, (
	void* handle, void* renderPass, void* framebuffer,
	glm::ivec2* offset, glm::ivec2* extent, MonoArray* clearValues
), Yonai.Graphics.Backends.Vulkan)
{
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = (VkRenderPass)renderPass;
	renderPassInfo.framebuffer = (VkFramebuffer)framebuffer;
	renderPassInfo.renderArea.offset = { offset->x, offset->y };
	renderPassInfo.renderArea.extent = { (unsigned int)extent->x, (unsigned int)extent->y };

	vector<VkClearValue> clearColours;
	clearColours.resize(mono_array_length(clearValues));
	for (size_t i = 0; i < clearColours.size(); i++)
	{
		VkClearValueManaged managed = mono_array_get(clearValues, VkClearValueManaged, i);
		if (managed.UseColour)
		{
			clearColours[i].color.float32[0] = managed.ColourR;
			clearColours[i].color.float32[1] = managed.ColourG;
			clearColours[i].color.float32[2] = managed.ColourB;
			clearColours[i].color.float32[3] = managed.ColourA;
		}
		else
			clearColours[i].depthStencil = managed.DepthStencil;
	}

	renderPassInfo.clearValueCount = (unsigned int)clearColours.size();
	renderPassInfo.pClearValues = clearColours.data();

	vkCmdBeginRenderPass((VkCommandBuffer)handle, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, EndRenderPass, void, (void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkCmdEndRenderPass((VkCommandBuffer)handle); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, BindVertexBuffers, void, (void* handle, MonoArray* inBuffers, MonoArray* inOffsets), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkBuffer> buffers;
	vector<VkDeviceSize> offsets;

	buffers.resize(mono_array_length(inBuffers));
	for(unsigned int i = 0; i < buffers.size(); i++)
		buffers[i] = mono_array_get(inBuffers, VkBuffer, i);

	offsets.resize(mono_array_length(inOffsets));
	for(unsigned int i = 0; i < offsets.size(); i++)
		offsets[i] = (VkDeviceSize)mono_array_get(inOffsets, int, i);

	vkCmdBindVertexBuffers((VkCommandBuffer)handle, 0, (unsigned int)buffers.size(), buffers.data(), offsets.data());
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, BindIndexBuffer, void, (void* handle, void* buffer, int offset, int indexType), Yonai.Graphics.Backends.Vulkan)
{ vkCmdBindIndexBuffer((VkCommandBuffer)handle, (VkBuffer)buffer, offset, (VkIndexType)indexType); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, CopyBuffer, void, (void* handle, void* src, void* dst, int srcOffset, int dstOffset, int size), Yonai.Graphics.Backends.Vulkan)
{
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer((VkCommandBuffer)handle, (VkBuffer)src, (VkBuffer)dst, 1, &copyRegion);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, BindDescriptorSets, void, (
	void* handle, int bindPoint, void* pipelineLayout, unsigned int firstSet, MonoArray* descriptorSets
), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkDescriptorSet> sets;
	sets.resize(mono_array_length(descriptorSets));
	for (size_t i = 0; i < sets.size(); i++)
		sets[i] = mono_array_get(descriptorSets, VkDescriptorSet, i);

	vkCmdBindDescriptorSets(
		(VkCommandBuffer)handle,
		(VkPipelineBindPoint)bindPoint,
		(VkPipelineLayout)pipelineLayout,
		firstSet,
		(unsigned int)sets.size(),
		sets.data(),
		0,
		nullptr
	);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, BlitImage, void, (
	VkCommandBuffer handle,
	VkImage srcImage, VkImageLayout srcLayout,
	VkImage dstImage, VkImageLayout dstLayout,
	MonoArray* inRegions, VkFilter filter),
	Yonai.Graphics.Backends.Vulkan)
{
	vector<VkImageBlit> regions;
	regions.resize(mono_array_length(inRegions));
	for (size_t i = 0; i < regions.size(); i++)
		regions[i] = mono_array_get(inRegions, VkImageBlit, i);

	vkCmdBlitImage(
		handle,
		srcImage, srcLayout,
		dstImage, dstLayout,
		(unsigned int)regions.size(),
		regions.data(),
		filter
	);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, PipelineBarrierImage, void, (void* handle, void* inBarrier, int srcStage, int dstStage), Yonai.Graphics.Backends.Vulkan)
{
	VkImageMemoryBarrier* barrier = (VkImageMemoryBarrier*)inBarrier;
	barrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

	vkCmdPipelineBarrier(
		(VkCommandBuffer)handle,
		(VkPipelineStageFlags)srcStage,
		(VkPipelineStageFlags)dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, barrier
	);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, CopyBufferToImage, void, (void* handle, void* inRegion, void* buffer, void* image), Yonai.Graphics.Backends.Vulkan)
{
	VkBufferImageCopy* region = (VkBufferImageCopy*)inRegion;

	vkCmdCopyBufferToImage(
		(VkCommandBuffer)handle,
		(VkBuffer)buffer,
		(VkImage)image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		region
	);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, ClearColorImage, void, (VkCommandBuffer handle, VkImage image, VkImageLayout layout, glm::vec4* inColour, MonoArray* inRanges), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkImageSubresourceRange> ranges;
	ranges.resize(mono_array_length(inRanges));
	for (size_t i = 0; i < ranges.size(); i++)
		ranges[i] = mono_array_get(inRanges, VkImageSubresourceRange, i);

	VkClearColorValue clearValue = { { inColour->r, inColour->g, inColour->b, inColour->a } };
	vkCmdClearColorImage(handle, image, layout, &clearValue, (unsigned int)ranges.size(), ranges.data());
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, Dispatch, void, (VkCommandBuffer handle, unsigned int x, unsigned int y, unsigned int z), Yonai.Graphics.Backends.Vulkan)
{ vkCmdDispatch(handle, x, y, z); }

ADD_MANAGED_METHOD(VulkanCommandBuffer, PushConstants, void, (
	VkCommandBuffer handle,
	VkPipelineLayout pipelineLayout,
	VkShaderStageFlags stageFlags,
	unsigned int offset,
	unsigned int size,
	void* values
), Yonai.Graphics.Backends.Vulkan)
{ vkCmdPushConstants(handle, pipelineLayout, stageFlags, offset, size, values);}

ADD_MANAGED_METHOD(VulkanFence, Create, void*, (void* device, int flags), Yonai.Graphics.Backends.Vulkan)
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = (VkFenceCreateFlagBits)flags;

	VkFence fence;
	VkResult result = vkCreateFence((VkDevice)device, &fenceInfo, nullptr, &fence);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create fence", result);
		return nullptr;
	}
	return fence;
}

struct VkRenderingInfoManaged
{
	VkRenderingFlags                    flags;
	VkRect2D                            renderArea;
	uint32_t                            layerCount;
	uint32_t                            viewMask;
};

struct VkRenderingAttachmentInfoManaged
{
	VkImageView              imageView;
	VkImageLayout            imageLayout;
	VkResolveModeFlagBits    resolveMode;
	VkImageView              resolveImageView;
	VkImageLayout            resolveImageLayout;
	VkAttachmentLoadOp       loadOp;
	VkAttachmentStoreOp      storeOp;
	VkClearValueManaged*     clearValue;
};

VkRenderingAttachmentInfo ParseManagedRenderingAttachment(VkRenderingAttachmentInfoManaged managed)
{
	VkRenderingAttachmentInfo attachmentInfo = {};
	attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachmentInfo.imageView = managed.imageView;
	attachmentInfo.imageLayout = managed.imageLayout;
	attachmentInfo.resolveMode = managed.resolveMode;
	attachmentInfo.resolveImageView = managed.resolveImageView;
	attachmentInfo.resolveImageLayout = managed.resolveImageLayout;
	attachmentInfo.loadOp = managed.loadOp;
	attachmentInfo.storeOp = managed.storeOp;

	if (managed.clearValue && managed.clearValue->UseColour)
	{
		attachmentInfo.clearValue.color =
		{
			managed.clearValue->ColourR,
			managed.clearValue->ColourG,
			managed.clearValue->ColourB,
			managed.clearValue->ColourA
		};
	}
	else if(managed.clearValue)
		attachmentInfo.clearValue.depthStencil = managed.clearValue->DepthStencil;

	return attachmentInfo;
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, BeginRendering, void, (
	VkCommandBuffer cmd,
	VkRect2D* renderArea,
	unsigned int layerCount,
	unsigned int viewMask,
	MonoArray* inColourAttachments,
	void* inDepthAttachment,
	void* inStencilAttachment
), Yonai.Graphics.Backends.Vulkan)
{
	VkRenderingAttachmentInfo depthAttachment;
	VkRenderingAttachmentInfo stencilAttachment;
	vector<VkRenderingAttachmentInfo> colourAttachments;

	colourAttachments.resize(mono_array_length(inColourAttachments));
	for (size_t i = 0; i < colourAttachments.size(); i++)
	{
		VkRenderingAttachmentInfoManaged managed = mono_array_get(inColourAttachments, VkRenderingAttachmentInfoManaged, i);
		colourAttachments[i] = ParseManagedRenderingAttachment(managed);
	}

	VkRenderingInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	info.colorAttachmentCount = (unsigned int)colourAttachments.size();
	info.pColorAttachments = colourAttachments.data();
	info.renderArea = *renderArea;
	info.layerCount = layerCount;
	info.viewMask = viewMask;

	if (inDepthAttachment)
	{
		depthAttachment = ParseManagedRenderingAttachment(*(VkRenderingAttachmentInfoManaged*)inDepthAttachment);
		info.pDepthAttachment = &depthAttachment;
	}

	if (inStencilAttachment)
	{
		stencilAttachment = ParseManagedRenderingAttachment(*(VkRenderingAttachmentInfoManaged*)inStencilAttachment);
		info.pStencilAttachment = &stencilAttachment;
	}

	vkCmdBeginRenderingKHR_(cmd, &info);
}

ADD_MANAGED_METHOD(VulkanCommandBuffer, EndRendering, void, (VkCommandBuffer cmd), Yonai.Graphics.Backends.Vulkan)
{ vkCmdEndRenderingKHR_(cmd); }

ADD_MANAGED_METHOD(VulkanFence, Destroy, void, (void* device, void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkDestroyFence((VkDevice)device, (VkFence)handle, nullptr); }

ADD_MANAGED_METHOD(VulkanFence, Wait, int, (void* device, MonoArray* handles, bool waitAll, unsigned int timeout), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkFence> fences;
	fences.resize(mono_array_length(handles));
	for (size_t i = 0; i < fences.size(); i++)
		fences[i] = mono_array_get(handles, VkFence, i);

	return vkWaitForFences((VkDevice)device, (unsigned int)fences.size(), fences.data(), waitAll, timeout);
}

ADD_MANAGED_METHOD(VulkanFence, Reset, int, (void* device, MonoArray* handles), Yonai.Graphics.Backends.Vulkan)
{
	vector<VkFence> fences;
	fences.resize(mono_array_length(handles));
	for (size_t i = 0; i < fences.size(); i++)
		fences[i] = mono_array_get(handles, VkFence, i);

	return vkResetFences((VkDevice)device, (unsigned int)fences.size(), fences.data());
}

ADD_MANAGED_METHOD(VulkanSemaphore, Create, void*, (void* device), Yonai.Graphics.Backends.Vulkan)
{
	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkSemaphore semaphore;
	VkResult result = vkCreateSemaphore((VkDevice)device, &info, nullptr, &semaphore);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create semaphore", result);
		return nullptr;
	}
	return semaphore;
}

ADD_MANAGED_METHOD(VulkanSemaphore, Destroy, void, (void* device, void* handle), Yonai.Graphics.Backends.Vulkan)
{ vkDestroySemaphore((VkDevice)device, (VkSemaphore)handle, nullptr); }

ADD_MANAGED_METHOD(VulkanQueue, Submit, int, (void* queue, void* fence, MonoArray* monoWaitSemaphores, MonoArray* monoStageMask, MonoArray* monoCommandBuffers, MonoArray* monoSignalSemaphores), Yonai.Graphics.Backends.Vulkan)
{
	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Wait semaphores //
	vector<VkSemaphore> waitSemaphores;
	waitSemaphores.resize(mono_array_length(monoWaitSemaphores));
	for (size_t i = 0; i < waitSemaphores.size(); i++)
		waitSemaphores[i] = mono_array_get(monoWaitSemaphores, VkSemaphore, i);

	info.pWaitSemaphores = waitSemaphores.data();
	info.waitSemaphoreCount = (unsigned int)waitSemaphores.size();

	// Command buffers //
	vector<VkCommandBuffer> buffers;
	buffers.resize(mono_array_length(monoCommandBuffers));
	for (size_t i = 0; i < buffers.size(); i++)
		buffers[i] = mono_array_get(monoCommandBuffers, VkCommandBuffer, i);

	info.pCommandBuffers = buffers.data();
	info.commandBufferCount = (unsigned int)buffers.size();

	// Signal semaphores //
	vector<VkSemaphore> signalSemaphores;
	signalSemaphores.resize(mono_array_length(monoSignalSemaphores));
	for (size_t i = 0; i < signalSemaphores.size(); i++)
		signalSemaphores[i] = mono_array_get(monoSignalSemaphores, VkSemaphore, i);

	info.pSignalSemaphores = signalSemaphores.data();
	info.signalSemaphoreCount = (unsigned int)signalSemaphores.size();

	// Stage flags //
	vector<VkPipelineStageFlags> stageFlags;
	if(monoStageMask)
		stageFlags.resize(mono_array_length(monoStageMask));
	for (size_t i = 0; i < stageFlags.size(); i++)
		stageFlags[i] = mono_array_get(monoStageMask, VkPipelineStageFlags, i);

	info.pWaitDstStageMask = stageFlags.data();
	
	return (int)vkQueueSubmit((VkQueue)queue, 1, &info, (VkFence)fence);
}

ADD_MANAGED_METHOD(VulkanQueue, Present, int, (void* queue, MonoArray* monoSemaphores, MonoArray* monoSwapchains, MonoArray* monoImageIndices), Yonai.Graphics.Backends.Vulkan)
{
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	// Wait semaphores //
	vector<VkSemaphore> waitSemaphores;
	waitSemaphores.resize(mono_array_length(monoSemaphores));
	for (size_t i = 0; i < waitSemaphores.size(); i++)
		waitSemaphores[i] = mono_array_get(monoSemaphores, VkSemaphore, i);

	info.pWaitSemaphores = waitSemaphores.data();
	info.waitSemaphoreCount = (unsigned int)waitSemaphores.size();
	
	// Swapchains //
	vector<VkSwapchainKHR> swapchains;
	swapchains.resize(mono_array_length(monoSwapchains));
	for (size_t i = 0; i < swapchains.size(); i++)
		swapchains[i] = mono_array_get(monoSwapchains, VkSwapchainKHR, i);

	info.pSwapchains = swapchains.data();
	info.swapchainCount = (unsigned int)swapchains.size();

	// Image indices //
	vector<unsigned int> imageIndices;
	imageIndices.resize(mono_array_length(monoImageIndices));
	for (size_t i = 0; i < imageIndices.size(); i++)
		imageIndices[i] = mono_array_get(monoImageIndices, unsigned int, i);

	info.pImageIndices = imageIndices.data();

	return vkQueuePresentKHR((VkQueue)queue, &info);
}

ADD_MANAGED_METHOD(VulkanQueue, WaitIdle, void, (void* queue), Yonai.Graphics.Backends.Vulkan)
{ vkQueueWaitIdle((VkQueue)queue); }

unsigned int FindMemoryType(VkPhysicalDevice device, unsigned int typeFilter, VkMemoryPropertyFlags propertyFlags)
{
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties((VkPhysicalDevice)device, &properties);
	
	for(unsigned int i = 0; i < properties.memoryTypeCount; i++)
		if(typeFilter & (1 << i) &&
			(properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
			return i;

	return UINT_MAX;
}

ADD_MANAGED_METHOD(VulkanBuffer, Create, int, (void* allocator, int bufferSize, VkBufferUsageFlags usage, int memoryUsage, VkBuffer* outputBuffer, void** outputAllocation), Yonai.Graphics.Backends.Vulkan)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaInfo = {};
	vmaInfo.usage = (VmaMemoryUsage)memoryUsage;
	vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	
	VmaAllocationInfo allocInfo;
	VkResult result = vmaCreateBuffer(
		(VmaAllocator)allocator,
		&bufferInfo,
		&vmaInfo,
		outputBuffer,
		(VmaAllocation*)outputAllocation,
		&allocInfo
	);

	return result;
}

ADD_MANAGED_METHOD(VulkanBuffer, Destroy, void, (void* allocator, VkBuffer buffer, void* allocation), Yonai.Graphics.Backends.Vulkan)
{ vmaDestroyBuffer((VmaAllocator)allocator, buffer, (VmaAllocation)allocation); }

ADD_MANAGED_METHOD(VulkanBuffer, Upload, void, (void* allocation, MonoArray* monoData), Yonai.Graphics.Backends.Vulkan)
{
	vector<unsigned char> data;
	data.resize(mono_array_length(monoData));
	for(unsigned int i = 0; i < data.size(); i++)
		data[i] = mono_array_get(monoData, unsigned char, i);

	memcpy(((VmaAllocation)allocation)->GetMappedData(), data.data(), data.size());
}

ADD_MANAGED_METHOD(VulkanBuffer, MapMemory, void*, (void* device, void* bufferMemory, int offset, int size), Yonai.Graphics.Backends.Vulkan)
{
	void* data;
	vkMapMemory((VkDevice)device, (VkDeviceMemory)bufferMemory, offset, size, 0, &data);
	return data;
}

ADD_MANAGED_METHOD(VulkanBuffer, UnmapMemory, void, (void* device, void* bufferMemory), Yonai.Graphics.Backends.Vulkan)
{ vkUnmapMemory((VkDevice)device, (VkDeviceMemory)bufferMemory); }

ADD_MANAGED_METHOD(VulkanImage, Create, int, (
	VkDevice device,
	void* allocator,
	VkImageCreateInfo* imageCreateInfo,
	VkImage* outImage,
	void** outAllocation
), Yonai.Graphics.Backends.Vulkan)
{
	*outImage = nullptr;
	*outAllocation = nullptr;

	imageCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VmaAllocation allocation;
	VkResult result = vmaCreateImage((VmaAllocator)allocator, imageCreateInfo, &allocInfo, outImage, &allocation, nullptr);

	*outAllocation = allocation;

	return result;
}
// _Destroy(IntPtr allocator, IntPtr image, IntPtr memoryAllocation);
ADD_MANAGED_METHOD(VulkanImage, Destroy, void, (void* allocator, VkImage image, void* memoryAllocation), Yonai.Graphics.Backends.Vulkan)
{ vmaDestroyImage((VmaAllocator)allocator, image, (VmaAllocation)memoryAllocation); }

ADD_MANAGED_METHOD(VulkanImage, CreateImageView, void*, (void* inDevice, void* image, VkImageViewCreateInfo* info), Yonai.Graphics.Backends.Vulkan)
{
	info->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	VkImageView imageView;
	VkResult result = vkCreateImageView((VkDevice)inDevice, info, nullptr, &imageView);
	if (result != VK_SUCCESS)
	{
		LogCriticalError("Failed to create image view", result);
		return nullptr;
	}

	return imageView;
}

ADD_MANAGED_METHOD(VulkanImage, DestroyImageView, void, (void* device, void* imageView), Yonai.Graphics.Backends.Vulkan)
{
	vkDestroyImageView((VkDevice)device, (VkImageView)imageView, nullptr);
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

ADD_MANAGED_METHOD(VulkanImage, CreateSampler, int, (void* device, VkSamplerCreateInfo* info, void** output), Yonai.Graphics.Backends.Vulkan)
{
	info->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	VkSampler sampler;
	VkResult result = vkCreateSampler((VkDevice)device, info, nullptr, &sampler);
	*output = sampler;
	return result;
}

ADD_MANAGED_METHOD(VulkanImage, DestroySampler, void, (void* device, void* sampler), Yonai.Graphics.Backends.Vulkan)
{ vkDestroySampler((VkDevice)device, (VkSampler)sampler, nullptr); }
