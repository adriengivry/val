/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vector>
#include <span>
#include <filesystem>
#include <vks/CommandBuffer.h>
#include <vks/sync/Semaphore.h>
#include <vks/sync/Fence.h>

namespace vks::utils
{
	class CommandBufferUtils
	{
	public:
		/**
		* Submit the given command buffers
		*/
		static void SubmitCommandBuffers(
			VkQueue p_queue,
			std::initializer_list<std::reference_wrapper<vks::CommandBuffer>> p_commandBuffers,
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_signalSemaphores,
			sync::Fence& p_fence
		);
	};
}
