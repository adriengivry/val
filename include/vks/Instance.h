/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <memory>
#include <string>
#include <span>
#include <vector>
#include <vks/DebugMessenger.h>
#include <vks/Surface.h>
#include <vks/SwapChain.h>
#include <vks/utils/DeviceManager.h>
#include <vks/utils/ValidationLayerManager.h>
#include <vulkan/vulkan.h>

namespace vks
{
	struct InstanceDesc
	{
		std::vector<std::string> requiredExtensions;
	};

	class Instance
	{
	public:
		/**
		* Creates a Vulkan Instance 
		*/
		Instance(const InstanceDesc& desc = {});

		/**
		* Destroys the Vulkan instance
		*/
		virtual ~Instance();

		/**
		* Returns the instance handle
		*/
		VkInstance GetHandle() const;

		/**
		* Returns enabled extensions
		*/
		const std::vector<const char*>& GetExtensions() const;

		/**
		* Returns enabled validation layers
		*/
		const std::vector<const char*>& GetValidationLayers() const;

	private:
		utils::ExtensionManager m_extensionManager;
		utils::ValidationLayerManager m_validationLayerManager;
		std::vector<const char*> m_extensions;
		std::vector<const char*> m_validationLayers;
		VkInstance m_handle = VK_NULL_HANDLE;
		std::unique_ptr<DebugMessenger> m_debugMessenger;
	};
}
