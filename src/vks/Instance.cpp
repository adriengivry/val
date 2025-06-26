/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/Instance.h>
#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>

namespace vks
{
	Instance::Instance(const InstanceDesc& desc)
	{
		m_extensionManager.FetchExtensions<utils::EExtensionHandler::Instance>();

		m_extensionManager.LogExtensions();
		m_validationLayerManager.LogValidationLayers();

		const bool k_useDebugUtilsExtension =
#ifdef DEBUG
			m_extensionManager.IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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

		m_extensions = m_extensionManager.FilterExtensions(requestedExtensions);
		m_validationLayers = m_validationLayerManager.FilterValidationLayers(requestedValidationLayers);

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
			.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size()), // determine the global validation layers to enable
			.ppEnabledLayerNames = m_validationLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size()),
			.ppEnabledExtensionNames = m_extensions.data()
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
	}

	Instance::~Instance()
	{
		m_debugMessenger.reset();
		vkDestroyInstance(m_handle, nullptr);
	}

	VkInstance Instance::GetHandle() const
	{
		assert(m_handle);
		return m_handle;
	}

	const std::vector<const char*>& Instance::GetExtensions() const
	{
		return m_extensions;
	}

	const std::vector<const char*>& Instance::GetValidationLayers() const
	{
		return m_validationLayers;
	}
}
