/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/ShaderProgram.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	ShaderProgram::ShaderProgram(std::initializer_list<const std::reference_wrapper<ShaderStage>> p_stages)
	{
		m_assembledStages.reserve(p_stages.size());
		for (auto& stage : p_stages)
		{
			m_assembledStages.push_back(stage.get().GetCreateInfo());
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> ShaderProgram::GetAssembledStages()
	{
		
		return m_assembledStages;
	}
}
