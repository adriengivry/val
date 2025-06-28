/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/ValidationLayerManager.h>
#include <vks/Device.h>
#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <set>
#include <limits>
#include <algorithm> 

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

	bool IsSwapChainAdequate(const vks::utils::SwapChainSupportDetails& p_swapChainSupportDetails)
	{
		return
			!p_swapChainSupportDetails.formats.empty() &&
			!p_swapChainSupportDetails.presentModes.empty();
	}
}

namespace vks
{
	bool QueueFamilyIndices::IsComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

	std::vector<uint32_t> QueueFamilyIndices::GetUniqueQueueIndices() const
	{
		assert(IsComplete());

		std::set<uint32_t> uniqueIndices{
			graphicsFamily.value(),
			presentFamily.value()
		};

		std::vector<uint32_t> output;
		output.reserve(uniqueIndices.size());

		for (uint32_t index : uniqueIndices)
		{
			output.push_back(index);
		}

		return output;
	}

	Device::Device(VkPhysicalDevice p_physicalDevice, VkSurfaceKHR p_surface) :
		m_physicalDevice(p_physicalDevice),
		m_queueFamilyIndices(FindQueueFamilies(p_physicalDevice, p_surface)),
		m_surface(p_surface)
	{
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);

		m_extensionManager.FetchExtensions<utils::EExtensionHandler::PhysicalDevice>(m_physicalDevice);

		// Based on the configuration of the device, we require some extensions.
		m_requestedExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME, true);
	}

	Device::~Device()
	{
		vkDestroyDevice(m_logicalDevice, nullptr);
	}

	void Device::QuerySuitability()
	{
		m_suitable = [this]() {
			if (!m_queueFamilyIndices.IsComplete())
			{
				return false;
			}

			// A device isn't suitable if any of the required extension is unavailable
			for (auto& extension : m_requestedExtensions)
			{
				if (extension.required && !m_extensionManager.IsExtensionSupported(extension.name))
				{
					return false;
				}
			}

			// Store swap chain support details since they can be used for swap chain creation
			m_swapChainSupportDetails = utils::SwapChainUtils::QuerySwapChainDetails(m_physicalDevice, m_surface);
			if (!IsSwapChainAdequate(m_swapChainSupportDetails))
			{
				return false;
			}

			// For example, we can require a physical device to be a discrete GPU 
			return m_physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		}();
	}

	bool Device::IsSuitable() const
	{
		return m_suitable;
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

		// Filter out all the non-supported extensions. All the required extensions should be available
		// since we checked for them in "IsSuitable()"
		std::vector<const char*> extensions = m_extensionManager.FilterExtensions(m_requestedExtensions);

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			// Deprecated validation layers on device
			// .enabledLayerCount = static_cast<uint32_t>(p_validationLayers.size()),
			// .ppEnabledLayerNames = p_validationLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
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
		assert(m_suitable);
		assert(m_logicalDevice != VK_NULL_HANDLE);
		return m_logicalDevice;
	}

	VkQueue Device::GetGraphicsQueue() const
	{
		assert(m_suitable);
		assert(m_logicalDevice != VK_NULL_HANDLE);
		assert(m_graphicsQueue != VK_NULL_HANDLE);
		return m_graphicsQueue;
	}

	VkQueue Device::GetPresentQueue() const
	{
		assert(m_suitable);
		assert(m_logicalDevice != VK_NULL_HANDLE);
		assert(m_presentQueue != VK_NULL_HANDLE);
		return m_presentQueue;
	}

	const utils::SwapChainSupportDetails& Device::GetSwapChainSupportDetails() const
	{
		assert(m_suitable);
		return m_swapChainSupportDetails;
	}

	const QueueFamilyIndices& Device::GetQueueFamilyIndices() const
	{
		return m_queueFamilyIndices;
	}

	void Device::WaitForFences(
		std::initializer_list<std::reference_wrapper<vks::sync::Fence>> p_fences,
		bool p_waitAll,
		std::optional<uint64_t> p_timeout
	)
	{
		std::vector<VkFence> fences;
		fences.reserve(p_fences.size());
		for (const auto& fence : p_fences)
		{
			fences.push_back(fence.get().GetHandle());
		}

		vkWaitForFences(
			m_logicalDevice,
			static_cast<uint32_t>(fences.size()),
			fences.data(),
			p_waitAll,
			p_timeout.value_or(std::numeric_limits<uint64_t>::max())
		);
	}

	void Device::ResetFences(
		std::initializer_list<std::reference_wrapper<vks::sync::Fence>> p_fences
	)
	{
		std::vector<VkFence> fences;
		fences.reserve(p_fences.size());
		for (const auto& fence : p_fences)
		{
			fences.push_back(fence.get().GetHandle());
		}

		vkResetFences(
			m_logicalDevice,
			static_cast<uint32_t>(fences.size()),
			fences.data()
		);
	}
	void Device::WaitForSemaphores(
		std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_semaphores,
		bool p_waitAll,
		std::optional<uint64_t> p_timeout
	)
	{
		std::vector<VkSemaphore> semaphores;
		semaphores.reserve(p_semaphores.size());
		for (const auto& semaphore : p_semaphores)
		{
			semaphores.push_back(semaphore.get().GetHandle());
		}

		VkSemaphoreWaitInfo waitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = static_cast<uint32_t>(semaphores.size()),
			.pSemaphores = semaphores.data()
		};

		vkWaitSemaphores(
			m_logicalDevice,
			&waitInfo,
			p_timeout.value_or(std::numeric_limits<uint64_t>::max())
		);
	}
}
