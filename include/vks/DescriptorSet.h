/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <span>

namespace vks
{
	class Buffer;
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

		/**
		* Attaches a list of buffers and images to the descriptor set.
		*/
		void Write(
			VkDescriptorType p_type,
			std::span<const std::reference_wrapper<Buffer>> p_buffers
		);

	private:
		DescriptorSet(VkDevice p_device, VkDescriptorSet p_handle);

		friend class DescriptorPool;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkDescriptorSet m_handle = VK_NULL_HANDLE;
	};
}
