/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/Queue.h>
#include <vks/SwapChain.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
	// Input need to be a class with GetHandle() (VkObject)
	template<class Output, class Input>
	std::vector<Output> PrepareArray(std::initializer_list<std::reference_wrapper<Input>> p_elements)
	{
		std::vector<Output> output;
		output.reserve(p_elements.size());
		for (const auto& element : p_elements)
		{
			output.push_back(element.get().GetHandle());
		}
		return output;
	}
}

namespace vks
{
	Queue::Queue(VkDevice p_device, VkQueue p_queue) :
		m_device(p_device),
		m_handle(p_queue)
	{
	}

	void Queue::Submit(
		std::initializer_list<std::reference_wrapper<vks::CommandBuffer>> p_commandBuffers,
		std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
		std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_signalSemaphores,
		sync::Fence& p_fence
	)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const auto waitSemaphores = PrepareArray<VkSemaphore>(p_waitSemaphores);
		const auto signalSemaphores = PrepareArray<VkSemaphore>(p_signalSemaphores);
		const auto commandBuffers = PrepareArray<VkCommandBuffer>(p_commandBuffers);

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

		if (vkQueueSubmit(m_handle, 1, &submitInfo, p_fence.GetHandle()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit queue!");
		}
	}

	void Queue::Present(
		std::initializer_list<std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
		vks::SwapChain& p_swapChain,
		uint32_t p_swapChainIndice
	)
	{
		const auto waitSemaphores = PrepareArray<VkSemaphore>(p_waitSemaphores);
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
		if (vkQueuePresentKHR(m_handle, &presentInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit queue!");
		}
	}

	VkQueue Queue::GetHandle() const
	{
		return m_handle;
	}
}
