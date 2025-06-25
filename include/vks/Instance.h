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
		Instance(const InstanceDesc& desc = {});
		virtual ~Instance();

	private:
		VkInstance m_handle = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		std::unique_ptr<DebugMessenger> m_debugMessenger;
	};
}
