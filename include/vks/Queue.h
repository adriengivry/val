/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <vks/sync/Fence.h>
#include <vks/sync/Semaphore.h>
#include <vks/CommandBuffer.h>

namespace vks
{
	class SwapChain;

	class Queue
	{
	public:
		/**
		* Creates a queue from an existing handle
		*/
		Queue(VkDevice p_device, VkQueue p_queue);

		/**
		* Destroys the queue 
		*/
		virtual ~Queue() = default;

		/**
		* Submit the queue
		*/
		void Submit(
			std::initializer_list<std::reference_wrapper<vks::CommandBuffer>> p_commandBuffers,
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_signalSemaphores,
			sync::Fence& p_fence
		);

		void Present(
			std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
			vks::SwapChain& p_swapChain,
			uint32_t p_swapChainIndice
		);

		/**
		* Returns the VkQueue handle
		*/
		VkQueue GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkQueue m_handle = VK_NULL_HANDLE;
	};
}
