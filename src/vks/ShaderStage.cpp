/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/ShaderStage.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	ShaderStage::ShaderStage(const ShaderModule& p_shaderModule, VkShaderStageFlagBits p_stage) :
		m_shaderStageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = p_stage,
			.module = p_shaderModule.GetHandle(),
			.pName = "main"
		}
	{

	}

	const VkPipelineShaderStageCreateInfo& ShaderStage::GetCreateInfo()
	{
		return m_shaderStageCreateInfo;
	}
}
