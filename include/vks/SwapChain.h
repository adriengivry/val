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
#include <vks/Framebuffer.h>
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
			VkExtent2D p_extent,
			const utils::SwapChainOptimalConfig& p_desc
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
		const utils::SwapChainOptimalConfig& GetDesc() const;

		/**
		* Returns the swap chain extent
		*/
		VkExtent2D GetExtent() const;

		/**
		* Create framebuffers for each image
		*/
		std::vector<vks::Framebuffer> CreateFramebuffers(VkRenderPass p_renderPass);

	private:
		Device& m_device;
		utils::SwapChainOptimalConfig m_desc;
		VkExtent2D m_extent;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	};
}
