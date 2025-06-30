/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <val/Queue.h>
#include <val/SwapChain.h>
#include <val/utils/MemoryUtils.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace val
{
	Queue::Queue(VkDevice p_device, VkQueue p_queue) :
		m_device(p_device),
		m_handle(p_queue)
	{
	}

	void Queue::Submit(
		std::initializer_list<std::reference_wrapper<val::CommandBuffer>> p_commandBuffers,
		std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_waitSemaphores,
		std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_signalSemaphores,
		std::optional<std::reference_wrapper<sync::Fence>> p_fence
	)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const auto waitSemaphores = utils::MemoryUtils::PrepareArray<VkSemaphore>(p_waitSemaphores);
		const auto signalSemaphores = utils::MemoryUtils::PrepareArray<VkSemaphore>(p_signalSemaphores);
		const auto commandBuffers = utils::MemoryUtils::PrepareArray<VkCommandBuffer>(p_commandBuffers);

		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
			.pWaitSemaphores = waitSemaphores.data(),
			.pWaitDstStageMask = waitStages,
			.commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
			.pCommandBuffers = commandBuffers.data(),
			.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
			.pSignalSemaphores = signalSemaphores.data()
		};

		if (vkQueueSubmit(
			m_handle,
			1,
			&submitInfo,
			p_fence.has_value() ? p_fence.value().get().GetHandle() : VK_NULL_HANDLE
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit queue!");
		}
	}

	void Queue::Present(
		std::initializer_list<std::reference_wrapper<val::sync::Semaphore>> p_waitSemaphores,
		val::SwapChain& p_swapChain,
		uint32_t p_swapChainIndice
	)
	{
		const auto waitSemaphores = utils::MemoryUtils::PrepareArray<VkSemaphore>(p_waitSemaphores);
		const auto swapChainHandle = p_swapChain.GetHandle();

		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
			.pWaitSemaphores = waitSemaphores.data(),
			.swapchainCount = 1,
			.pSwapchains = &swapChainHandle,
			.pImageIndices = &p_swapChainIndice,
			.pResults = nullptr // (optional) allows to specify an array of VkResult values to check for every individual swap chain if presentation was successful. 
		};

		// Note: there is an error with our semaphores that can be addressed with:
		// https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
		// It seems like the debug validation layer didn't use to pick up this error when vulkan-tutorial was written.
		VkResult result = vkQueuePresentKHR(m_handle, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			throw OutOfDateSwapChain();
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit queue!");
		}
	}

	VkQueue Queue::GetHandle() const
	{
		return m_handle;
	}
}
