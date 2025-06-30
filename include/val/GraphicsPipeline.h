/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <val/ShaderProgram.h>
#include <val/SwapChain.h>
#include <val/RenderPass.h>
#include <val/DescriptorSetLayout.h>

namespace val
{
	struct GraphicsPipelineDesc
	{
		ShaderProgram& program;
		RenderPass& renderPass;
		std::span<const VkVertexInputAttributeDescription> vertexInputAttributeDesc;
		std::span<const VkVertexInputBindingDescription> vertexInputBindingDesc;
		std::span<const std::reference_wrapper<DescriptorSetLayout>> descriptorSetLayouts;
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

		/**
		* Returns a VkPipelineLayout handle 
		*/
		VkPipelineLayout GetLayout() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	};
}
