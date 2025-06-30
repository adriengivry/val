/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <val/ShaderStage.h>
#include <vector>

namespace val
{
	class ShaderProgram
	{
	public:
		/**
		* Creates a shader program
		*/
		ShaderProgram(std::initializer_list<const std::reference_wrapper<ShaderStage>> p_stages);

		/**
		* Destroys the shader program
		*/
		virtual ~ShaderProgram() = default;

		/**
		* Returns a contiguous array with create info for each shader stage
		*/
		std::vector<VkPipelineShaderStageCreateInfo> GetAssembledStages();

	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_assembledStages;
	};
}
