/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/utils/SwapChainUtils.h>
#include <vks/Device.h>

namespace vks
{
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
