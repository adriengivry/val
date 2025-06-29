/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>

namespace vks
{
	class DescriptorPool;

	class DescriptorSet
	{
	public:
		/**
		* Destroys the descriptor set
		*/
		virtual ~DescriptorSet() = default;

		/**
		* Returns the descriptor set handle
		*/
		VkDescriptorSet GetHandle() const;

	private:
		DescriptorSet(VkDescriptorSet p_handle);

		friend class DescriptorPool;

	private:
		VkDescriptorSet m_handle = VK_NULL_HANDLE;
	};
}
