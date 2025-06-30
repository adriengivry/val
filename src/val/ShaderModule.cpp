/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/ShaderModule.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	ShaderModule::ShaderModule(VkDevice p_device, const std::span<const std::byte> p_byteCode) :
		m_device(p_device)
	{
		VkShaderModuleCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = p_byteCode.size(),
			.pCode = reinterpret_cast<const uint32_t*>(p_byteCode.data())
		};

		if (vkCreateShaderModule(
			m_device,
			&createInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

	}

	ShaderModule::~ShaderModule()
	{
		vkDestroyShaderModule(m_device, m_handle, nullptr);
	}

	VkShaderModule ShaderModule::GetHandle() const
	{
		return m_handle;
	}
}
