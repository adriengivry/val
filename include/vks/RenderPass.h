/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <vks/SwapChain.h>

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

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkRenderPass m_handle = VK_NULL_HANDLE;
	};
}
