/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <val/utils/SwapChainUtils.h>
#include <val/sync/Semaphore.h>
#include <val/sync/Fence.h>
#include <val/Framebuffer.h>
#include <stdexcept>

namespace val
{
	class Device;

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
		* Create framebuffers for each image
		*/
		std::vector<val::Framebuffer> CreateFramebuffers(VkRenderPass p_renderPass);

	private:
		Device& m_device;
		utils::SwapChainOptimalConfig m_desc;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	};
}
