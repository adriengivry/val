/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>
#include <span>
#include <ranges>

namespace
{
	VkInstance g_instance;
	VkDebugUtilsMessengerEXT g_debugMessenger;

	struct RequestedValidationLayer
	{
		std::string name;
		bool required;
	};

	struct RequestedExtension
	{
		std::string name;
		bool required;
	};

	std::vector<VkExtensionProperties> getSupportedExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		return extensions;
	}

	std::vector<VkLayerProperties> getSupportedValidationLayers()
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		return availableLayers;
	}

	bool isExtensionSupported(const std::string_view extension, std::span<const VkExtensionProperties> supportedExtensions)
	{
		for (auto& layer : supportedExtensions)
		{
			if (strcmp(layer.extensionName, extension.data()) == 0)
			{
				return true;
			}
		}

		return false;
	}

	bool isValidationLayerSupported(const std::string_view validationLayer, std::span<const VkLayerProperties> supportedValidationLayers)
	{
		for (auto& layer : supportedValidationLayers)
		{
			if (strcmp(layer.layerName, validationLayer.data()) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void printExtensions(std::span<const VkExtensionProperties> extensions)
	{
		std::cout << extensions.size() << " available extensions:\n";

		for (const auto& extension : extensions)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}

	void printValidationLayers(std::span<const VkLayerProperties> layers)
	{
		std::cout << layers.size() << " available validation layers:\n";

		for (const auto& layer : layers)
		{
			std::cout << '\t' << layer.layerName << '\n';
		}
	}

	auto getMessageOutputStream(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
	{
		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return stdout;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return stdout;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return stdout;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return stderr;
		}

		return stdout;
	}

	std::string getMessageSeverityHeader(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
	{
		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "[vk verbose]";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return "[vk info]";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "[vk warning]";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return "[vk error]";
		}

		return "[vk unknown]";
	}

	std::string getMessageTypeHeader(VkDebugUtilsMessageTypeFlagsEXT type)
	{
		switch (type)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: return "<general>";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: return "<performance>";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: return "<validation>";
		}

		return "<unknown>";
	}

	/**
	* Returns a boolean that indicates if the Vulkan call that triggered the validation layer message should be aborted.
	* If the callback returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error.
	* This is normally only used to test the validation layers themselves, so you should always return VK_FALSE.
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		// Ignore info messages (too noisy).
		// Could also be achieved by not using "VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT" when 
		// creating the DebugUtilsMessageCreateInfo instance
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			return VK_FALSE;
		}

		const auto stream = getMessageOutputStream(messageSeverity);
		const std::string severityHeader = getMessageSeverityHeader(messageSeverity);
		const std::string typeHeader = getMessageTypeHeader(messageType);
		const std::string message = pCallbackData->pMessage;

		fprintf(
			stream,
			"%s %s: %s\n",
			severityHeader.data(),
			typeHeader.data(),
			message.data()
		);
		
		return VK_FALSE;
	}

	// Unfortunately, because this function is an extension function, it is not automatically loaded.
	// We have to look up its address ourselves using vkGetInstanceProcAddr.
	// We're going to create our own proxy function that handles this in the background
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	// Same as above, since this is an extension function, it is not automatically loaded.
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	// This has been moved to a function so that it can be used by `setupDebugMessenger` (regular debug messenger creation)
	// and passed to the creation info of the instance (so that vkCreateInstance can be debugged).
	VkDebugUtilsMessengerCreateInfoEXT CreateDebugUtilsMessageCreateInfo()
	{
		return {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | // could be removed to get less noise
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debugCallback,
			.pUserData = nullptr // Optional
		};
	}

	void setupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateDebugUtilsMessageCreateInfo();

		if (CreateDebugUtilsMessengerEXT(g_instance, &createInfo, nullptr, &g_debugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void initVulkan()
	{
		// Retrieve Vulkan extensions
		const auto supportedExtensions = getSupportedExtensions();
		const auto supportedValidationLayers = getSupportedValidationLayers();

		printExtensions(supportedExtensions);
		printValidationLayers(supportedValidationLayers);

		// Retrieve extensions required by GLFW (array of strings)
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<RequestedExtension> requestedExtensions{
#ifdef DEBUG
			{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME, false } // (optional) extension for message callback
#endif
		};

		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			requestedExtensions.emplace_back(glfwExtensions[i], true); // "true" to make it required
		}

		std::vector<const char*> extensions;
		extensions.reserve(requestedExtensions.size());

		// Filter unsupported extensions
		for (const auto& extension : requestedExtensions)
		{
			if (!isExtensionSupported(extension.name, supportedExtensions))
			{
				if (extension.required)
				{
					throw std::runtime_error("required extension not supported!");
				}
				else
				{
					std::cout << "Optional requested extension '" << extension.name << "' isn't supported: skipped\n";
				}
			}
			else
			{
				extensions.push_back(extension.name.c_str());
			}
		}

		// Check if the debug utils extension is being used.
		const bool isDebugUtilsExtensionPresent = [&extensions] {
			for (auto& extension : extensions)
			{
				if (strcmp(extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
				{
					return true;
				}
			}

			return false;
		}();

		std::vector<RequestedValidationLayer> requestedValidationLayers{
#ifdef DEBUG
			{ "VK_LAYER_KHRONOS_validation", false }
#endif
		};

		std::vector<const char*> validationLayers;
		validationLayers.reserve(requestedValidationLayers.size());

		// Filter unsupported validation layers
		for (const auto& layer : requestedValidationLayers)
		{
			if (!isValidationLayerSupported(layer.name, supportedValidationLayers))
			{
				if (layer.required)
				{
					throw std::runtime_error("required layer not supported!");
				}
				else
				{
					std::cout << "Optional requested validation layer '" << layer.name << "' isn't supported: skipped\n";
				}
			}
			else
			{
				validationLayers.push_back(layer.name.c_str());
			}
		}

		// VkApplicationInfo is technically optional, but useful to specify info about our app.
		VkApplicationInfo appInfo{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = VK_NULL_HANDLE, // To pass additionnal information, e.g. for extensions
			.pApplicationName = "vulkan-sandbox",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "No Engine",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_0
		};

		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()), // determine the global validation layers to enable
			.ppEnabledLayerNames = validationLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		// Since the debug messenger is created after the vkInstance is created, it won't catch issues
		// related to instance creation.
		// To fix that, we add an instance of VkDebugUtilsMessengerCreateInfoEXT to createInfo.pNext.
		if (isDebugUtilsExtensionPresent)
		{
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = CreateDebugUtilsMessageCreateInfo();
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugUtilsCreateInfo;
		}

		VkResult result = vkCreateInstance(
			&createInfo, // pointer to instace of VkXxxxxCreateInfo
			nullptr, // allocator callback (nullptr)
			&g_instance // pointer to output
		);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
		else
		{
			std::cout << "Vulkan instance created successfully!\n";
		}

		// If the debug utils extension is being used, setup the debug messenger
		if (isDebugUtilsExtensionPresent)
		{
			setupDebugMessenger();
		}
	}

	void deinitVulkan()
	{
		if (g_debugMessenger != VK_NULL_HANDLE)
		{
			DestroyDebugUtilsMessengerEXT(g_instance, g_debugMessenger, nullptr);
		}

		vkDestroyInstance(g_instance, nullptr);
	}
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "vulkan-sandbox", nullptr, nullptr);

	initVulkan();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	deinitVulkan();

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}
