/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include <vks/Device.h>

namespace vks::utils
{
	class DeviceManager
	{
	public:
		/**
		* Creates the device manager
		*/
		DeviceManager(VkInstance p_instance);

		/**
		* Destroys the device manager
		*/
		virtual ~DeviceManager() = default;

		/**
		* Returns a suitable device
		*/
		Device& GetSuitableDevice();

	private:
		std::vector<Device> m_devices;
	};
}
