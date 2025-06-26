/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/ExtensionManager.h>
#include <vks/utils/ValidationLayerManager.h>
#include <vks/Device.h>
#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <set>

namespace
{
	vks::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR p_surface)
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

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, p_surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
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
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

	Device::Device(VkPhysicalDevice p_physicalDevice, VkSurfaceKHR p_surface) :
		m_physicalDevice(p_physicalDevice),
		m_queueFamilyIndices(FindQueueFamilies(p_physicalDevice, p_surface))
	{
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);
	}

	Device::~Device()
	{
		vkDestroyDevice(m_logicalDevice, nullptr);
	}

	bool Device::IsSuitable() const
	{
		if (!m_queueFamilyIndices.IsComplete())
		{
			return false;
		}

		// For example, we can require a physical device to be a discrete GPU 
		return m_physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	}

	void Device::CreateLogicalDevice(std::vector<const char*> p_validationLayers)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {
			m_queueFamilyIndices.graphicsFamily.value(),
			m_queueFamilyIndices.presentFamily.value()
		};

		float queuePriority = 1.0f;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = static_cast<uint32_t>(p_validationLayers.size()),
			.ppEnabledLayerNames = p_validationLayers.data(),
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = VK_NULL_HANDLE,
			.pEnabledFeatures = &m_physicalDeviceFeatures,
		};

		if (vkCreateDevice(
			m_physicalDevice,
			&createInfo,
			nullptr,
			&m_logicalDevice
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_logicalDevice, m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_logicalDevice, m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
	}

	VkPhysicalDevice Device::GetPhysicalDevice() const
	{
		assert(m_physicalDevice != VK_NULL_HANDLE);
		return m_physicalDevice;
	}

	VkDevice Device::GetLogicalDevice() const
	{
		assert(m_logicalDevice != VK_NULL_HANDLE);
		return m_logicalDevice;
	}

	VkQueue Device::GetGraphicsQueue() const
	{
		assert(m_logicalDevice != VK_NULL_HANDLE);
		assert(m_graphicsQueue != VK_NULL_HANDLE);
		return m_graphicsQueue;
	}

	VkQueue Device::GetPresentQueue() const
	{
		assert(m_logicalDevice != VK_NULL_HANDLE);
		assert(m_presentQueue != VK_NULL_HANDLE);
		return m_presentQueue;
	}
}
