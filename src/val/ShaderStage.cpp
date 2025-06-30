/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/ShaderStage.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
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

	const VkPipelineShaderStageCreateInfo& ShaderStage::GetCreateInfo() const
	{
		return m_shaderStageCreateInfo;
	}
}
