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
		Device(VkPhysicalDevice p_physicalDevice);

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
		void CreateLogicalDevice();

		/**
		* Returns the logical device associated with this physical device
		* @note CreateLogicalDevice() must be called before calling GetLogicalDevice()
		*/
		VkDevice GetLogicalDevice();

	private:
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices m_queueFamilyIndices;
	};
}
