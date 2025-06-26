/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/DeviceManager.h>
#include <iostream>

namespace vks::utils
{
	DeviceManager::DeviceManager(VkInstance p_instance, VkSurfaceKHR p_surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(p_instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(p_instance, &deviceCount, devices.data());

		m_devices.reserve(devices.size());

		// Log devices
		for (auto device : devices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			std::cout << "Device found: " << deviceProperties.deviceName << std::endl;
			m_devices.emplace_back(device, p_surface);
		}

		// Cache suitability info about each device
		for (auto& device : m_devices)
		{
			device.QuerySuitability();
		}
	}

	Device& vks::utils::DeviceManager::GetSuitableDevice()
	{
		// Alternatively, we could rank devices based on their features, and pick the best device for the task.
		// Or let the user select a device.
		// For more details: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
		for (auto& device : m_devices)
		{
			if (device.IsSuitable())
			{
				return device;
			}
		}

		throw std::runtime_error("failed to find a suitable GPU!");
	}
}
