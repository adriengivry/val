/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/Framebuffer.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	Framebuffer::Framebuffer(VkDevice p_device, const FramebufferDesc& p_desc) :
		m_device(p_device)
	{
		VkFramebufferCreateInfo framebufferInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = p_desc.renderPass,
			.attachmentCount = 1,
			.pAttachments = p_desc.attachments.data(),
			.width = p_desc.extent.width,
			.height = p_desc.extent.height,
			.layers = 1
		};

		if (vkCreateFramebuffer(
			m_device,
			&framebufferInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	Framebuffer::~Framebuffer()
	{
		vkDestroyFramebuffer(m_device, m_handle, nullptr);
	}

	VkFramebuffer Framebuffer::GetHandle() const
	{
		return m_handle;
	}
}
