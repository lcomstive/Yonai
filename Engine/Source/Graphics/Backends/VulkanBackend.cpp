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
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <glfw/glfw3native.h>
#include <vulkan/vk_enum_string_helper.h>
#include <Yonai/Graphics/Backends/VulkanBackend.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics::Backends;

const bool VulkanBackend::EnableValidationLayers =
#ifndef NDEBUG
true;
#else
false;
#endif
const vector<const char*> VulkanBackend::ValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#pragma region Forward Declarations
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);
#pragma endregion

void LogCriticalError(const char* msg, VkResult result)
{
	spdlog::critical("[Vulkan] {} - {}", msg, string_VkResult(result));
	exit((int)result);
}

VulkanBackend::VulkanBackend() : Instance()
{
	spdlog::debug("Initialising Vulkan backend");

	CreateInstance();
	GetAvailableExtensions();
	SetupDebugMessenger();

	CreateSurface();

	FindPhysicalDevices();
	if (AvailablePhysicalDevices.empty())
		return;
	SelectDevice(AvailablePhysicalDevices[0]);
}

VulkanBackend::~VulkanBackend()
{
	spdlog::debug("Destroying Vulkan backend");
	Cleanup();
}

void VulkanBackend::Cleanup()
{
	vkDestroyDevice(Device, nullptr);

	if (EnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(Instance, m_DebugMessenger, nullptr);

	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkDestroyInstance(Instance, nullptr);
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

bool VulkanBackend::CheckValidationLayerSupport(const vector<const char*>& validationLayers)
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

vector<const char*> VulkanBackend::GetRequiredExtensions()
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

#pragma region Devices
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

	QueueFamilyIndices indices = FindQueueFamilies(device);
	if (!indices.IsComplete())
		return -1; // Does not have desired queue families

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

	for(const auto& device : devices)
	{
		int score = RateDeviceSuitability(device);
		if (score > 0)
			candidates.insert(make_pair(score, device));
	}

	if (candidates.rbegin()->first <= 0)
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
	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

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

	createInfo.enabledExtensionCount = 0;

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

#pragma region Queue Families
VulkanBackend::QueueFamilyIndices VulkanBackend::FindQueueFamilies(VkPhysicalDevice device)
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
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Surface, &presentSupport);

		if (presentSupport)
			indices.PresentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}
#pragma endregion

#pragma region Debug Info
VkDebugUtilsMessengerCreateInfoEXT VulkanBackend::CreateDebugInfo()
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
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		spdlog::info ("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	spdlog::warn ("[Vulkan][{}] {}", messageTypeStr, pCallbackData->pMessage); break;
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