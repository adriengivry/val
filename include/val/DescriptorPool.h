/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <list>
#include <vector>

namespace val
{
	class CommandBuffer;
	class Device;
	class DescriptorSet;
	class DescriptorSetLayout;

	class DescriptorPool
	{
	public:
		/**
		* Creates a descriptor pool
		*/
		DescriptorPool(Device& p_device);

		/**
		* Destroys the descriptor pool
		*/
		virtual ~DescriptorPool();

		/**
		* Allocates descriptor sets
		*/
		std::vector<std::reference_wrapper<DescriptorSet>> AllocateDescriptorSets(
			const DescriptorSetLayout& p_layout,
			uint32_t p_count
		);

		/**
		* Returns the descriptor pool handle
		*/
		VkDescriptorPool GetHandle() const;

	private:
		Device& m_device;
		VkDescriptorPool m_handle = VK_NULL_HANDLE;
		std::list<DescriptorSet> m_descriptorSets;
	};
}
