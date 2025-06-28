/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/sync/Semaphore.h>
#include <span>
#include <vector>
#include <optional>

namespace vks::sync
{
	class SemaphoreWaitGroup
	{
	public:
		/**
		* Creates a semaphore wait group
		*/
		SemaphoreWaitGroup(
			VkDevice p_device,
			std::span<const std::reference_wrapper<Semaphore>> p_syncObjects,
			std::optional<uint64_t> p_timeout
		);

		/**
		* Destroys the semaphore wait group
		*/
		virtual ~SemaphoreWaitGroup();

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		std::vector<VkSemaphore> m_syncObjects;
	};
}
