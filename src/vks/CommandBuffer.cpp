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
}
