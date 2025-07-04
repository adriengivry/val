/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/DescriptorPool.h>
#include <val/DescriptorSet.h>
#include <val/DescriptorSetLayout.h>
#include <val/Device.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	DescriptorPool::DescriptorPool(val::Device& p_device, uint32_t p_maxSetCount) :
		m_device(p_device)
	{
		assert(p_maxSetCount > 0 && "Max set count must be greater than 0");

		VkDescriptorPoolSize poolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1
		};

		VkDescriptorPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = static_cast<uint32_t>(p_maxSetCount),
			.poolSizeCount = 1,
			.pPoolSizes = &poolSize,
		};

		if (vkCreateDescriptorPool(
			m_device.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool");
		}
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_device.GetLogicalDevice(), m_handle, nullptr);
	}

	std::vector<std::reference_wrapper<DescriptorSet>> DescriptorPool::AllocateDescriptorSets(
		const DescriptorSetLayout& p_layout,
		uint32_t p_count
	)
	{
		std::vector<VkDescriptorSetLayout> layouts(p_count, p_layout.GetHandle());

		std::vector<std::reference_wrapper<DescriptorSet>> output;
		output.reserve(p_count);

		VkDescriptorSetAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = m_handle,
			.descriptorSetCount = p_count,
			.pSetLayouts = layouts.data()
		};

		std::vector<VkDescriptorSet> allocatedDescriptorSets(p_count);

		if (vkAllocateDescriptorSets(
			m_device.GetLogicalDevice(),
			&allocInfo,
			allocatedDescriptorSets.data()) != VK_SUCCESS
		)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (auto allocatedDescriptorSet : allocatedDescriptorSets)
		{
			output.emplace_back(
				m_descriptorSets.emplace_back(DescriptorSet{
					m_device.GetLogicalDevice(),
					allocatedDescriptorSet
				})
			);
		}

		return output;
	}

	VkDescriptorPool DescriptorPool::GetHandle() const
	{
		return m_handle;
	}
}
