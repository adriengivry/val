/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/DescriptorSetLayout.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	DescriptorSetLayout::DescriptorSetLayout(VkDevice p_device, std::span<const DescriptorSetLayoutBinding> p_bindings) :
		m_device(p_device)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bindings.reserve(p_bindings.size());

		for (const auto& binding : p_bindings)
		{
			bindings.push_back(VkDescriptorSetLayoutBinding{
				.binding = binding.binding,
				.descriptorType = binding.type,
				.descriptorCount = 1,
				.stageFlags = binding.stageFlags,
				.pImmutableSamplers = nullptr // Optional
			});
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};

		VkResult result = vkCreateDescriptorSetLayout(
			m_device,
			&layoutInfo,
			nullptr,
			&m_handle
		);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device, m_handle, nullptr);
	}

	VkDescriptorSetLayout DescriptorSetLayout::GetHandle() const
	{
		return m_handle;
	}
}
