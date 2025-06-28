/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/sync/Fence.h>
#include <span>
#include <vector>
#include <optional>

namespace vks::sync
{
	class FenceWaitGroup
	{
	public:
		/**
		* Creates a fence wait group
		*/
		FenceWaitGroup(
			VkDevice p_device,
			std::span<const std::reference_wrapper<Fence>> p_syncObjects,
			bool p_waitAll = true,
			std::optional<uint64_t> p_timeout = std::nullopt
		);

		/**
		* Destroys the fence wait group
		*/
		virtual ~FenceWaitGroup();

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		std::vector<VkFence> m_syncObjects;
	};
}
