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

	struct ValidationLayerRequest
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

	bool isValidationLayerSupported(std::string_view validationLayer, std::span<const VkLayerProperties> supportedValidationLayers)
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

	void initVulkan()
	{
		// Retrieve Vulkan extensions
		const auto vkExtensions = getSupportedExtensions();
		printExtensions(vkExtensions);

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

		// Retrieve extensions from GLFW (array of strings)
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<ValidationLayerRequest> requestedValidationLayers = {
#ifdef DEBUG
			{ "VK_LAYER_KHRONOS_validation", false },
#endif
		};

		const auto supportedValidationLayers = getSupportedValidationLayers();

		std::vector<const char*> validationLayers;
		validationLayers.reserve(requestedValidationLayers.size());

		// Iterate over all unsupported validation layers to provide information to the user.
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

		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()), // determine the global validation layers to enable
			.ppEnabledLayerNames = validationLayers.data(),
			.enabledExtensionCount = glfwExtensionCount,
			.ppEnabledExtensionNames = glfwExtensions
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
