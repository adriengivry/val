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

	void CommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_handle, 0);
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

	void CommandBuffer::BeginRenderPass(
		VkRenderPass p_renderPass,
		VkFramebuffer p_framebuffer,
		VkExtent2D p_extent
	)
	{
		VkClearValue clearColor = { {
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		} };

		VkRenderPassBeginInfo renderPassInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = p_renderPass,
			.framebuffer = p_framebuffer,
			.renderArea = {
				.offset = { 0, 0 },
				.extent = p_extent
			},
			.clearValueCount = 1,
			.pClearValues = &clearColor
		};

		vkCmdBeginRenderPass(
			m_handle,
			&renderPassInfo,
			VK_SUBPASS_CONTENTS_INLINE
		);
	}

	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_handle);
	}

	void CommandBuffer::BindPipeline(VkPipelineBindPoint p_bindPoint, VkPipeline p_pipeline)
	{
		vkCmdBindPipeline(m_handle, p_bindPoint, p_pipeline);
	}

	void CommandBuffer::SetViewport(const VkViewport& p_viewport)
	{
		vkCmdSetViewport(m_handle, 0, 1, &p_viewport);
	}

	void CommandBuffer::SetScissor(const VkRect2D& p_scissor)
	{
		vkCmdSetScissor(m_handle, 0, 1, &p_scissor);
	}

	void CommandBuffer::Draw(uint32_t p_vertexCount, uint32_t p_instanceCount)
	{
		vkCmdDraw(m_handle, p_vertexCount, p_instanceCount, 0, 0);
	}
}
