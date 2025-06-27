/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vector>
#include <filesystem>
#include <vks/ShaderStage.h>

namespace vks::utils
{
	class ShaderUtils
	{
	public:
		/**
		* Returns a byte array containing the SPIR-V code of the shader file.
		*/
		static std::vector<std::byte> ReadShaderFile(const std::filesystem::path p_fileName);

		/**
		* Assemble shader stages and return a contiguous array with filled VkPipelineShaderStageCreateInfo structures
		*/
		static std::vector<VkPipelineShaderStageCreateInfo> AssembleShaderStages(std::span<const ShaderStage> p_stages);
	};
}
