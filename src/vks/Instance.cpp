/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/ExtensionManager.h>
#include <vks/utils/ValidationLayerManager.h>
#include <vks/Instance.h>
#include <stdexcept>
#include <iostream>

namespace
{
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// For example, we can require a physical device to be a discrete GPU and have support for geometry shaders
		return
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;
	}

	VkPhysicalDevice PickPhysicalDevice(VkInstance p_intance)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(p_intance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(p_intance, &deviceCount, devices.data());

		// Alternatively, we could rank devices based on their features, and pick the best device for the task.
		// Or let the user select a device.
		// For more details: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
		for (auto device : devices)
		{
			if (IsPhysicalDeviceSuitable(device))
			{
				return device;
			}
		}

		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

namespace vks
{
	Instance::Instance(const InstanceDesc& desc)
	{
		utils::ExtensionManager extensionManager;
		utils::ValidationLayerManager validationLayerManager;

		extensionManager.LogExtensions();
		validationLayerManager.LogValidationLayers();

		const bool k_useDebugUtilsExtension =
#ifdef DEBUG
			extensionManager.IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#else
			false;
#endif

		std::vector<utils::RequestedExtension> requestedExtensions;
		
		// (optional) extension for message callback
		if (k_useDebugUtilsExtension)
		{
			requestedExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true);
		};

		for (auto& extension : desc.requiredExtensions)
		{
			requestedExtensions.emplace_back(extension, true); // "true" to make it required
		}

		std::vector<utils::RequestedValidationLayer> requestedValidationLayers;

		if (k_useDebugUtilsExtension)
		{
			requestedValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation", false);
		}

		const auto extensions = extensionManager.FilterExtensions(requestedExtensions);
		const auto validationLayers = validationLayerManager.FilterValidationLayers(requestedValidationLayers);

		// VkApplicationInfo is optional, but useful to specify info about our app.
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

		std::unique_ptr<VkDebugUtilsMessengerCreateInfoEXT> debugUtilsMessengerCreateInfo;

		// Since the debug messenger is created after the vkInstance is created, it won't catch issues
		// related to instance creation.
		// To fix that, we add an instance of VkDebugUtilsMessengerCreateInfoEXT to createInfo.pNext.
		if (k_useDebugUtilsExtension)
		{
			debugUtilsMessengerCreateInfo = std::make_unique<VkDebugUtilsMessengerCreateInfoEXT>(
				DebugMessengerUtil::GenerateCreateInfo()
			);

			createInfo.pNext = debugUtilsMessengerCreateInfo.get();
		}

		VkResult result = vkCreateInstance(
			&createInfo, // pointer to instace of VkXxxxxCreateInfo
			nullptr, // allocator callback (nullptr)
			&m_handle // pointer to output
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
		if (k_useDebugUtilsExtension)
		{
			m_debugMessenger = std::make_unique<DebugMessenger>(m_handle, *debugUtilsMessengerCreateInfo);
		}

		m_physicalDevice = PickPhysicalDevice(m_handle);
	}

	Instance::~Instance()
	{
		m_debugMessenger.reset();
		vkDestroyInstance(m_handle, nullptr);
	}
}
