/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/sync/Semaphore.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks::sync
{
	Semaphore::Semaphore(VkDevice p_device) :
		m_device(p_device)
	{
		VkSemaphoreCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};

		if (vkCreateSemaphore(m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence");
		}
	}

	Semaphore::~Semaphore()
	{
		vkDestroySemaphore(m_device, m_handle, nullptr);
	}

	VkSemaphore Semaphore::GetHandle() const
	{
		return m_handle;
	}
}
