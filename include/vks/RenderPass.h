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
	class RenderPass
	{
	public:
		/**
		* Creates a render pass
		*/
		RenderPass(VkDevice p_device, VkFormat p_format);

		/**
		* Destroys the render pass
		*/
		virtual ~RenderPass();

		/**
		* Returns the underlying VkRenderPass handle
		*/
		VkRenderPass GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkRenderPass m_handle = VK_NULL_HANDLE;
	};
}
