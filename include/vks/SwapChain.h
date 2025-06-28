/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/utils/SwapChainUtils.h>
#include <vks/sync/Semaphore.h>
#include <vks/sync/Fence.h>
#include <vks/Device.h>
#include <stdexcept>

namespace vks
{
	class OutOfDateSwapChain : public std::runtime_error
	{
	public:
		OutOfDateSwapChain() : std::runtime_error("Swap chain out of date") {}
	};

	class SwapChain
	{
	public:
		/**
		* Creates a swap chain
		*/
		SwapChain(
			Device& p_device,
			VkSurfaceKHR p_surface,
			const utils::SwapChainDesc& p_desc
		);

		/**
		* Destroys the swap chain
		*/
		virtual ~SwapChain();

		/**
		* Returns the underlying VkSwapchainKHR handle
		*/
		VkSwapchainKHR GetHandle() const;

		/**
		* Returns the index of the next image
		* @note throw an exception if the swapchain is out of date
		*/
		uint32_t AcquireNextImage(
			std::optional<std::reference_wrapper<sync::Semaphore>> p_semaphore = std::nullopt,
			std::optional<std::reference_wrapper<sync::Fence>> p_fence = std::nullopt,
			std::optional<uint64_t> p_timeout = std::nullopt
		);

		/**
		* Returns swap chain images
		*/
		const std::vector<VkImage>& GetImages() const;

		/**
		* Returns the swap chain desc
		*/
		const utils::SwapChainDesc& GetDesc() const;

	private:
		Device& m_device;
		utils::SwapChainDesc m_desc;
		std::vector<VkImage> m_images;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	};
}
