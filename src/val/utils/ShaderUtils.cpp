/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/utils/ShaderUtils.h>
#include <cassert>
#include <fstream>

namespace val::utils
{
	std::vector<std::byte> ShaderUtils::ReadShaderFile(const std::filesystem::path p_fileName)
	{
		assert(std::filesystem::exists(p_fileName));

		std::ifstream file(p_fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open shader file: " + p_fileName.string());
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<std::byte> buffer(fileSize);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
		file.close();
		return buffer;
	}
}
