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
#include <array>
#include <vector>
#include <vks/DebugMessenger.h>
#include <vks/utils/ExtensionManager.h>
#include <vks/utils/SwapChainUtils.h>
#include <vks/sync/Fence.h>
#include <vks/sync/Semaphore.h>
#include <vks/Queue.h>
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

		/**
		* Returns a contiguous array of indices
		*/
		std::vector<uint32_t> GetUniqueQueueIndices() const;
	};

	// TODO: Separate Physical and Logical device
	class Device
	{
	public:
		/**
		* Create a device instance from a physical device
		*/
		Device(VkPhysicalDevice p_physicalDevice, VkSurfaceKHR p_surface);

		/**
		* Copy constructor
		*/
		Device(const Device& p_rhs);

		/**
		* Destroys the device
		*/
		virtual ~Device();

		/**
		* Checks if the device is suitable, and cache the result
		*/
		void QuerySuitability();

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
		Queue GetGraphicsQueue() const;

		/**
		* Returns the present queue associated with this logical device
		* @note will assert if the device doesn't have a logical device associated
		*/
		Queue GetPresentQueue() const;

		/**
		* Returns swap chain support details for this physical device
		*/
		const utils::SwapChainSupportDetails& GetSwapChainSupportDetails() const;

		/**
		* Returns queue family indices
		*/
		const QueueFamilyIndices& GetQueueFamilyIndices() const;

		/**
		* Wait for fences
		*/
		void WaitForFences(
			std::initializer_list<std::reference_wrapper<vks::sync::Fence>> p_fences,
			bool p_waitAll = true,
			std::optional<uint64_t> p_timeout = std::nullopt
		);

		/**
		* Reset fences
		*/
		void ResetFences(
			std::initializer_list<std::reference_wrapper<vks::sync::Fence>> p_fences
		);

		/**
		* Wait for semaphores
		*/
		void WaitForSemaphores(
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_semaphores,
			bool p_waitAll = true,
			std::optional<uint64_t> p_timeout = std::nullopt
		);

		/**
		* Wait idle
		*/
		void WaitIdle();

	private:
		bool m_suitable = false;
		std::vector<utils::RequestedExtension> m_requestedExtensions;
		utils::ExtensionManager m_extensionManager;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		std::unique_ptr<Queue> m_graphicsQueue;
		std::unique_ptr<Queue> m_presentQueue;
		QueueFamilyIndices m_queueFamilyIndices;
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;
		utils::SwapChainSupportDetails m_swapChainSupportDetails;
	};
}
