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
#include <vks/utils/DeviceManager.h>
#include <vulkan/vulkan.h>

namespace vks
{
	struct InstanceDesc
	{
		std::vector<std::string> requiredExtensions;
		std::optional<SurfaceDesc> surfaceDesc;
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
		* Returns the surface associated with this instance
		*/
		const Surface& GetSurface() const;

	private:
		VkInstance m_handle = VK_NULL_HANDLE;
		std::unique_ptr<utils::DeviceManager> m_deviceManager;
		std::optional<std::reference_wrapper<vks::Device>> m_device;
		std::unique_ptr<Surface> m_surface;
		std::unique_ptr<DebugMessenger> m_debugMessenger;
	};
}
