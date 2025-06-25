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
			{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true } // Require extension for message callback
#endif
		};

		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			requestedExtensions.push_back(RequestedExtension{
					.name = glfwExtensions[i],
					.required = true
				}
			);
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
	}

	void deinitVulkan()
	{
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
