/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <span>

namespace vks
{
	class CommandPool;
	class Buffer;

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
		void Begin(VkCommandBufferUsageFlags p_flags = 0);

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
		* Copy buffer content from source to destination
		*/
		void CopyBuffer(Buffer& p_src, Buffer& p_dest, std::span<const VkBufferCopy> p_regions = {});

		/**
		* Bind a graphics pipeline
		*/
		void BindPipeline(VkPipelineBindPoint p_bindPoint, VkPipeline p_pipeline);

		/**
		* Bind index buffer
		*/
		void BindIndexBuffer(
			const Buffer& p_indexBuffer,
			uint64_t p_offset = 0,
			VkIndexType p_indexType = VkIndexType::VK_INDEX_TYPE_UINT32
		);

		/**
		* Bind vertex buffers
		*/
		void BindVertexBuffers(
			std::span<const std::reference_wrapper<Buffer>> p_buffers,
			std::span<const uint64_t> p_offsets
		);

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
		void Draw(uint32_t p_vertexCount, uint32_t p_instanceCount = 1);

		/**
		* Submit an indexed draw command
		*/
		void DrawIndexed(uint32_t p_indexCount, uint32_t p_instanceCount = 1);


	private:
		CommandBuffer(VkCommandBuffer p_handle);

		friend class CommandPool;

	private:
		VkCommandBuffer m_handle = VK_NULL_HANDLE;
	};
}
