/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <span>
#include <optional>

namespace vks
{
	struct FramebufferDesc
	{
		std::span<const VkImageView> attachments;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkExtent2D extent;
	};

	class Framebuffer
	{
	public:
		/**
		* Creates a framebuffer
		*/
		Framebuffer(
			VkDevice p_device,
			const FramebufferDesc& p_desc
		);

		/**
		* Destroys the framebuffer
		*/
		virtual ~Framebuffer();

		/**
		* Returns the underlying VkFramebuffer handle
		*/
		VkFramebuffer GetHandle() const;

	private:
		VkDevice m_device;
		VkFramebuffer m_handle = VK_NULL_HANDLE;
	};
}
