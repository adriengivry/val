/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/RenderPass.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	RenderPass::RenderPass(VkDevice p_device, VkFormat p_format) :
		m_device(p_device)
	{
		VkAttachmentDescription colorAttachment{
			.format = p_format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentRef{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			// The index of the attachment in this array is directly referenced from the fragment shader
			// with the layout(location = 0) out vec4 outColor directive!
			// Here we have an array of one element (no actual array, just ptr to element)
			.pColorAttachments = &colorAttachmentRef
		};

		VkSubpassDependency dependency{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		};

		VkRenderPassCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &colorAttachment,
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency
		};

		if (vkCreateRenderPass(
			m_device,
			&createInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	RenderPass::~RenderPass()
	{
		vkDestroyRenderPass(m_device, m_handle, nullptr);
	}

	VkRenderPass RenderPass::GetHandle() const
	{
		return m_handle;
	}
}
