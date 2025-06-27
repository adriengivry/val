/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/CommandBuffer.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	CommandBuffer::CommandBuffer(VkCommandBuffer p_commandBuffer) :
		m_handle(p_commandBuffer)
	{
	}

	VkCommandBuffer CommandBuffer::GetHandle() const
	{
		return m_handle;
	}

	void CommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0, // Optional
			.pInheritanceInfo = nullptr // Optional
		};

		if (vkBeginCommandBuffer(m_handle, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void CommandBuffer::End()
	{
		if (vkEndCommandBuffer(m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}
