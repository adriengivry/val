/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <memory>
#include <string>
#include <optional>
#include <span>
#include <vector>
#include <vks/DebugMessenger.h>
#include <vulkan/vulkan.h>

namespace vks
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		/**
		* Returns true if all the required queue family are complete
		*/
		bool IsComplete() const;
	};

	class Device
	{
	public:
		/**
		* Create a device instance from a physical device
		*/
		Device(VkPhysicalDevice p_physicalDevice, VkSurfaceKHR p_surface);

		/**
		* Destroys the device
		*/
		virtual ~Device();

		/**
		* Returns true if this device is suitable
		*/
		bool IsSuitable() const;

		/**
		* Creates the logical device for the current physical device
		*/
		void CreateLogicalDevice(std::vector<const char*> p_validationLayers);

		/**
		* Returns the physical device handle
		*/
		VkPhysicalDevice GetPhysicalDevice() const;

		/**
		* Returns the logical device associated with this physical device
		* @note CreateLogicalDevice() must be called before calling GetLogicalDevice()
		*/
		VkDevice GetLogicalDevice() const;

		/**
		* Returns the graphics queue associated with this logical device
		* @note will assert if the device doesn't have a logical device associated
		*/
		VkQueue GetGraphicsQueue() const;

		/**
		* Returns the present queue associated with this logical device
		* @note will assert if the device doesn't have a logical device associated
		*/
		VkQueue GetPresentQueue() const;

	private:
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue m_presentQueue = VK_NULL_HANDLE;
		QueueFamilyIndices m_queueFamilyIndices;
	};
}
