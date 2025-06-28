/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vks
{
	class CommandPool;

	class CommandBuffer
	{
	public:
		/**
		* Destroys the command buffer
		*/
		virtual ~CommandBuffer() = default;

		/**
		* Returns the command buffer handle
		*/
		VkCommandBuffer GetHandle() const;

		/**
		* Resets the command buffer
		*/
		void Reset();

		/**
		* Begin recording commands
		*/
		void Begin();

		/**
		* Finish recording commands
		*/
		void End();

		/**
		* Begin a render pass
		*/
		void BeginRenderPass(
			VkRenderPass p_renderPass,
			VkFramebuffer p_framebuffer,
			VkExtent2D p_extent
		);

		/**
		* End a render pass
		*/
		void EndRenderPass();

		/**
		* Bind a graphics pipeline
		*/
		void BindPipeline(VkPipelineBindPoint p_bindPoint, VkPipeline p_pipeline);

		/**
		* Set viewport
		*/
		void SetViewport(const VkViewport& p_viewport);

		/**
		* Set scissor
		*/
		void SetScissor(const VkRect2D& p_scissor);

		/**
		* Submit a draw command
		*/
		void Draw(uint32_t p_vertexCount, uint32_t p_instanceCount);

	private:
		CommandBuffer(VkCommandBuffer p_handle);

		friend class CommandPool;

	private:
		VkCommandBuffer m_handle = VK_NULL_HANDLE;
	};
}
