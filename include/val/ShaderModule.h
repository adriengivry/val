/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>

namespace val
{
	class ShaderModule
	{
	public:
		/**
		* Creates a shader module
		*/
		ShaderModule(VkDevice p_device, const std::span<const std::byte> p_byteCode);

		/**
		* Destroys the shader module
		*/
		virtual ~ShaderModule();

		/**
		* Returns the underlying VkShaderModule handle
		*/
		VkShaderModule GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkShaderModule m_handle = VK_NULL_HANDLE;
	};
}
