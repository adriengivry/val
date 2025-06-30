/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <val/ShaderModule.h>
#include <vulkan/vulkan.h>

namespace val
{
	class ShaderStage
	{
	public:
		/**
		* Creates a shader stage
		*/
		ShaderStage(const ShaderModule& p_shaderModule, VkShaderStageFlagBits p_stage);

		/**
		* Destroys the shader stage
		*/
		virtual ~ShaderStage() = default;

		/**
		* Returns the create info of the shader stage
		*/
		const VkPipelineShaderStageCreateInfo& GetCreateInfo() const;

	private:
		VkPipelineShaderStageCreateInfo m_shaderStageCreateInfo;
	};
}
