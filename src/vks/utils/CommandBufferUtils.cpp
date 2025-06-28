/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/CommandBufferUtils.h>
#include <cassert>
#include <fstream>

namespace
{
	std::vector<VkSemaphore> PrepareSemaphoreArray(std::span<const std::reference_wrapper<vks::sync::Semaphore>> p_semaphores)
	{
		std::vector<VkSemaphore> output;
		output.reserve(p_semaphores.size());
		for (const auto& semaphore : p_semaphores)
		{
			output.push_back(semaphore.get().GetHandle());
		}
		return output;
	}

	std::vector<VkCommandBuffer> PrepareCommandBufferArray(std::span<const std::reference_wrapper<vks::CommandBuffer>> p_commandBuffers)
	{
		std::vector<VkCommandBuffer> output;
		output.reserve(p_commandBuffers.size());
		for (const auto& commandBuffer : p_commandBuffers)
		{
			output.push_back(commandBuffer.get().GetHandle());
		}
		return output;
	}
}
namespace vks::utils
{
	void CommandBufferUtils::SubmitCommandBuffers(
		VkQueue p_queue,
		std::span<const std::reference_wrapper<vks::CommandBuffer>> p_commandBuffers,
		std::span<const std::reference_wrapper<vks::sync::Semaphore>> p_waitSemaphores,
		std::span<const std::reference_wrapper<vks::sync::Semaphore>> p_signalSemaphores,
		vks::sync::Fence& p_fence
	)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const auto waitSemaphores = PrepareSemaphoreArray(p_waitSemaphores);
		const auto signalSemaphores = PrepareSemaphoreArray(p_signalSemaphores);
		const auto commandBuffers = PrepareCommandBufferArray(p_commandBuffers);

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

		if (vkQueueSubmit(p_queue, 1, &submitInfo, p_fence.GetHandle()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
	}
}
