/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/ExtensionManager.h>
#include <vks/utils/ValidationLayerManager.h>
#include <vks/Device.h>
#include <stdexcept>
#include <iostream>
#include <optional>

namespace
{
	vks::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
	{
		vks::QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		uint32_t i = 0;

		for (const auto& queueFamily : queueFamilies)
		{
			// Early exit if all family queues have been identified
			if (indices.IsComplete())
			{
				break;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			++i;
		}

		return indices;
	}
}

namespace vks
{
	bool QueueFamilyIndices::IsComplete() const
	{
		return graphicsFamily.has_value();
	}

	Device::Device(VkPhysicalDevice p_physicalDevice) :
		m_physicalDevice(p_physicalDevice),
		m_queueFamilyIndices(FindQueueFamilies(p_physicalDevice))
	{
	}

	Device::~Device()
	{
		
	}

	bool Device::IsSuitable() const
	{
		if (!m_queueFamilyIndices.IsComplete())
		{
			return false;
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &deviceFeatures);

		// For example, we can require a physical device to be a discrete GPU 
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	}
}
