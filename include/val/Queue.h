/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <optional>
#include <vulkan/vulkan.h>
#include <val/sync/Fence.h>
#include <val/sync/Semaphore.h>
#include <val/CommandBuffer.h>

namespace val
{
	class SwapChain;

	class Queue
	{
	public:
		/**
		* Destroys the queue 
		*/
		virtual ~Queue() = default;

		/**
		* Submit the queue
		*/
		void Submit(
			std::initializer_list<std::reference_wrapper<val::CommandBuffer>> p_commandBuffers,
			std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_waitSemaphores = {},
			std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_signalSemaphores = {},
			std::optional<std::reference_wrapper<sync::Fence>> p_fence = std::nullopt
		);

		void Present(
			std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_waitSemaphores,
			val::SwapChain& p_swapChain,
			uint32_t p_swapChainIndice
		);

		/**
		* Returns the VkQueue handle
		*/
		VkQueue GetHandle() const;

	private:
		Queue(VkDevice p_device, VkQueue p_queue);

		friend class Device;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkQueue m_handle = VK_NULL_HANDLE;
	};
}
