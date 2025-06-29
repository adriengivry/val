/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/DescriptorSet.h>
#include <vks/Buffer.h>
#include <vks/utils/MemoryUtils.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	DescriptorSet::DescriptorSet(VkDescriptorSet p_descriptorSet) :
		m_handle(p_descriptorSet)
	{
	}

	VkDescriptorSet DescriptorSet::GetHandle() const
	{
		return m_handle;
	}
}
