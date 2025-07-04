/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vector>
#include <filesystem>
#include <val/ShaderStage.h>

namespace val::utils
{
	class ShaderUtils
	{
	public:
		/**
		* Returns a byte array containing the SPIR-V code of the shader file.
		*/
		static std::vector<std::byte> ReadShaderFile(const std::filesystem::path p_fileName);
	};
}
