/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/GraphicsPipeline.h>
#include <val/utils/ShaderUtils.h>
#include <val/utils/MemoryUtils.h>
#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace val
{
	GraphicsPipeline::GraphicsPipeline(VkDevice p_device, const GraphicsPipelineDesc& p_desc) :
		m_device(p_device)
	{
		// Collect shader stages
		const auto stages = p_desc.program.GetAssembledStages();

		const auto dynamicStates = std::to_array({
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		});

		// While most of the pipeline state needs to be baked into the pipeline state, a limited amount of the state can actually be
		// changed without recreating the pipeline at draw time. Examples are the size of the viewport, line width and blend constants.
		// If you want to use dynamic state and keep these properties out, then you'll have to fill in a VkPipelineDynamicStateCreateInfo
		// structure like this:
		VkPipelineDynamicStateCreateInfo dynamicState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};

		// Describes the format of the vertex data that will be passed to the vertex shader. It describes this in roughly two ways:
		//	Bindings: spacing between data and whether the data is per - vertex or per - instance(see instancing)
		//	Attribute descriptions : type of the attributes passed to the vertex shader, which binding to load them from and at which offset
		VkPipelineVertexInputStateCreateInfo vertexInputState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = static_cast<uint32_t>(p_desc.vertexInputBindingDesc.size()),
			.pVertexBindingDescriptions = p_desc.vertexInputBindingDesc.data(), // Optional
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(p_desc.vertexInputAttributeDesc.size()),
			.pVertexAttributeDescriptions = p_desc.vertexInputAttributeDesc.data() // Optional
		};

		// The VkPipelineInputAssemblyStateCreateInfo struct describes two things:
		// - what kind of geometry will be drawn from the vertices
		// - if primitive restart should be enabled.
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkPipelineViewportStateCreateInfo viewportState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = VK_NULL_HANDLE, // Dynamic viewport (VK_DYNAMIC_STATE_VIEWPORT)
			.scissorCount = 1,
			.pScissors = VK_NULL_HANDLE // Dynamic scissor (VK_DYNAMIC_STATE_SCISSOR)
		};

		VkPipelineRasterizationStateCreateInfo rasterizationState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f, // Optional
			.depthBiasClamp = 0.0f, // Optional
			.depthBiasSlopeFactor = 0.0f, // Optional
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampleState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f, // Optional
			.pSampleMask = nullptr, // Optional
			.alphaToCoverageEnable = VK_FALSE, // Optional
			.alphaToOneEnable = VK_FALSE // Optional
		};

		// If you are using a depth and/or stencil buffer, then you also need to configure the depth and stencil tests using
		// VkPipelineDepthStencilStateCreateInfo. We don't have one right now, so we can simply pass a nullptr instead of a
		// pointer to such a struct. We'll get back to it in the depth buffering chapter.

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
			.colorBlendOp = VK_BLEND_OP_ADD, // Optional
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
			.alphaBlendOp = VK_BLEND_OP_ADD, // Optional
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		};

		VkPipelineColorBlendStateCreateInfo colorBlendState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachmentState,
			.blendConstants = {
				0.0f, // Optional
				0.0f, // Optional
				0.0f, // Optional
				0.0f // Optional
			}
		};

		const auto descriptorSetLayouts = utils::MemoryUtils::PrepareArray<VkDescriptorSetLayout>(p_desc.descriptorSetLayouts);
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
			.pushConstantRangeCount = 0, // Optional
			.pPushConstantRanges = nullptr // Optional
		};

		if (vkCreatePipelineLayout(
			m_device,
			&pipelineLayoutInfo,
			nullptr,
			&m_pipelineLayout
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = static_cast<uint32_t>(stages.size()),
			.pStages = stages.data(),
			.pVertexInputState = &vertexInputState,
			.pInputAssemblyState = &inputAssemblyState,
			.pViewportState = &viewportState,
			.pRasterizationState = &rasterizationState,
			.pMultisampleState = &multisampleState,
			.pColorBlendState = &colorBlendState,
			.pDynamicState = &dynamicState,
			.layout = m_pipelineLayout,
			.renderPass = p_desc.renderPass.GetHandle(),
		};

		if (vkCreateGraphicsPipelines(
			m_device,
			VK_NULL_HANDLE,
			1,
			&createInfo,
			nullptr,
			&m_graphicsPipeline
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	}

	VkPipeline GraphicsPipeline::GetHandle() const
	{
		return m_graphicsPipeline;
	}

	VkPipelineLayout GraphicsPipeline::GetLayout() const
	{
		return m_pipelineLayout;
	}
}
