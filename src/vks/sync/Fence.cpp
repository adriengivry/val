/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/sync/Fence.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks::sync
{
	Fence::Fence(VkDevice p_device, bool p_createSignaled) :
		m_device(p_device)
	{
		VkFenceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = p_createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags{}
		};

		if (vkCreateFence(m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence");
		}
	}

	Fence::~Fence()
	{
		vkDestroyFence(m_device, m_handle, nullptr);
	}

	VkFence Fence::GetHandle() const
	{
		return m_handle;
	}
}
