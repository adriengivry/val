/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/sync/SemaphoreWaitGroup.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks::sync
{
	SemaphoreWaitGroup::SemaphoreWaitGroup(
		VkDevice p_device,
		std::span<const std::reference_wrapper<Semaphore>> p_syncObjects,
		std::optional<uint64_t> p_timeout
	) :
		m_device(p_device)
	{
		m_syncObjects.reserve(p_syncObjects.size());
		for (const auto& object : p_syncObjects)
		{
			m_syncObjects.push_back(object.get().GetHandle());
		}

		VkSemaphoreWaitInfo waitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.flags = 0, // optional
			.semaphoreCount = static_cast<uint32_t>(m_syncObjects.size()),
			.pSemaphores = m_syncObjects.data(),
		};

		if (!m_syncObjects.empty())
		{
			vkWaitSemaphores(
				m_device,
				&waitInfo,
				p_timeout.value_or(UINT64_MAX)
			);
		}
	}

	SemaphoreWaitGroup::~SemaphoreWaitGroup()
	{
		if (!m_syncObjects.empty())
		{
			// ?
		}
	}
}
