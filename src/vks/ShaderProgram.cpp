/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/ShaderProgram.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	ShaderProgram::ShaderProgram(std::span<const ShaderStage> p_stages) :
		m_stages(p_stages)
	{
	}

	std::vector<VkPipelineShaderStageCreateInfo> ShaderProgram::GetAssembledStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> assembledStages;
		assembledStages.reserve(m_stages.size());
		for (auto& stage : m_stages)
		{
			assembledStages.push_back(stage.GetCreateInfo());
		}
		return assembledStages;
	}
}
