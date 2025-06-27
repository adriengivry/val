/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <vks/SwapChain.h>
#include <vks/Framebuffer.h>
#include <vks/CommandBuffer.h>

namespace vks
{
	struct RenderPassDesc
	{
		SwapChain& swapChain;
	};

	class RenderPass
	{
	public:
		/**
		* Creates a shader module
		*/
		RenderPass(VkDevice p_device, const RenderPassDesc& p_renderPassDesc);

		/**
		* Destroys the shader module
		*/
		virtual ~RenderPass();

		/**
		* Returns the underlying VkRenderPass handle
		*/
		VkRenderPass GetHandle() const;

		/**
		* Begins the render pass
		*/
		void Begin(vks::CommandBuffer& p_commandBuffer, vks::Framebuffer& p_framebuffer, VkExtent2D p_extent);

		/**
		* Ends the render pass
		*/
		void End(vks::CommandBuffer& p_commandBuffer);

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkRenderPass m_handle = VK_NULL_HANDLE;
		std::optional<std::reference_wrapper<CommandBuffer>> m_currentCommandBuffer;
	};
}
