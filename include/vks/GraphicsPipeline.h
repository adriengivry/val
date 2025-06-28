/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/ShaderProgram.h>
#include <vks/SwapChain.h>
#include <vks/RenderPass.h>

namespace vks
{
	struct GraphicsPipelineDesc
	{
		ShaderProgram& program;
		RenderPass& renderPass;
	};

	class GraphicsPipeline
	{
	public:
		/**
		* Creates a graphics pipeline
		*/
		GraphicsPipeline(VkDevice p_device, const GraphicsPipelineDesc& p_desc);

		/**
		* Destroys the graphics pipeline
		*/
		virtual ~GraphicsPipeline();

		/**
		* Returns a VkPipeline handle
		*/
		VkPipeline GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	};
}
