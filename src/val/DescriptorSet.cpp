/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/DescriptorSet.h>
#include <val/Buffer.h>
#include <val/utils/MemoryUtils.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace val
{
	DescriptorSet::DescriptorSet(VkDevice p_device, VkDescriptorSet p_descriptorSet) :
		m_device(p_device),
		m_handle(p_descriptorSet)
	{
	}

	VkDescriptorSet DescriptorSet::GetHandle() const
	{
		return m_handle;
	}

	void DescriptorSet::Write(
		VkDescriptorType p_type,
		std::span<const std::reference_wrapper<Buffer>> p_buffers
	)
	{
		std::vector<VkDescriptorBufferInfo> bufferInfos;
		bufferInfos.reserve(p_buffers.size());

		for (auto& buffer : p_buffers)
		{
			VkDescriptorBufferInfo bufferInfo{
				.buffer = buffer.get().GetHandle(),
				.offset = 0,
				.range = VK_WHOLE_SIZE
			};

			bufferInfos.push_back(bufferInfo);
		}
		
		VkWriteDescriptorSet descriptorWrite{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = m_handle,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = p_type,
			.pImageInfo = nullptr, // Optional
			.pBufferInfo = bufferInfos.data(),
			.pTexelBufferView = nullptr, // Optional
		};

		vkUpdateDescriptorSets(
			m_device,
			1,
			&descriptorWrite,
			0,
			nullptr
		);
	}
}
