/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/SwapChain.h>
#include <stdexcept>

namespace
{
	uint32_t CalculateSwapChainImageCount(VkSurfaceCapabilitiesKHR p_capabilities)
	{
		// Simply sticking to this minimum means that we may sometimes have to wait on the driver
		// to complete internal operations before we can acquire another image to render to.
		// Therefore it is recommended to request at least one more image than the minimum:
		const uint32_t minImageCount = p_capabilities.minImageCount + 1;
		const uint32_t maxImageCount = p_capabilities.maxImageCount;

		if (maxImageCount > 0 && minImageCount > maxImageCount)
		{
			return maxImageCount;
		}

		return minImageCount;
	}
}

namespace vks
{
	SwapChain::SwapChain(
		Device& p_device,
		VkSurfaceKHR p_surface,
		const utils::SwapChainDesc& p_desc
	) :
		m_device(p_device),
		m_desc(p_desc)
	{
		const auto& queueFamilyIndices = p_device.GetQueueFamilyIndices();
		const auto indices = queueFamilyIndices.GetUniqueQueueIndices();

		VkSwapchainCreateInfoKHR createInfo{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = p_surface,
			.minImageCount = CalculateSwapChainImageCount(m_desc.capabilities),
			.imageFormat = m_desc.surfaceFormat.format,
			.imageColorSpace = m_desc.surfaceFormat.colorSpace,
			.imageExtent = m_desc.extent,
			.imageArrayLayers = 1, // always 1 unless we're developing a stereoscopic 3D application.

			 // It is also possible that we'll render images to a separate image first to perform operations like post-processing.
			 // In that case we may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer
			 // the rendered image to a swap chain image.
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

			// Specify how to handle swap chain images that will be used across multiple queue families.
			// That will be the case in our application if the graphics queue family is different from the presentation queue.
			// We'll be drawing on the images in the swap chain from the graphics queue and then submitting them on the presentation queue. 
			// 
			//	VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transferred before using it in another queue family.This option offers the best performance.
			//	VK_SHARING_MODE_CONCURRENT : Images can be used across multiple queue families without explicit ownership transfers.
			//
			.imageSharingMode = indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = indices.size() > 1 ? static_cast<uint32_t>(indices.size()) : 0,
			.pQueueFamilyIndices = indices.size() > 1 ? indices.data() : VK_NULL_HANDLE,

			.preTransform = m_desc.capabilities.currentTransform,

			// The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system. You'll almost always want to simply ignore the alpha channel, hence VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = m_desc.presentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE
		};

		if (vkCreateSwapchainKHR(
			m_device.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&m_swapChain
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(m_device.GetLogicalDevice(), m_swapChain, &imageCount, nullptr);
		m_images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device.GetLogicalDevice(), m_swapChain, &imageCount, m_images.data());
	}

	SwapChain::~SwapChain()
	{
		vkDestroySwapchainKHR(m_device.GetLogicalDevice(), m_swapChain, nullptr);
	}

	const std::vector<VkImage>& SwapChain::GetImages() const
	{
		return m_images;
	}

	const utils::SwapChainDesc& SwapChain::GetDesc() const
	{
		return m_desc;
	}
}
