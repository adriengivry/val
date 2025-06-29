/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/CommandPool.h>
#include <vks/CommandBuffer.h>
#include <vks/Device.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	CommandPool::CommandPool(vks::Device& p_device) :
		m_device(p_device)
	{
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_device.GetQueueFamilyIndices().graphicsFamily.value()
		};

		if (vkCreateCommandPool(
			m_device.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool");
		}
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(m_device.GetLogicalDevice(), m_handle, nullptr);
	}

	std::vector<std::reference_wrapper<CommandBuffer>> CommandPool::AllocateCommandBuffers(uint32_t p_count, VkCommandBufferLevel p_level)
	{
		std::vector<std::reference_wrapper<CommandBuffer>> output;
		output.reserve(p_count);

		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = m_handle,
			.level = p_level,
			.commandBufferCount = p_count
		};

		std::vector<VkCommandBuffer> allocatedCommandBuffers(p_count);

		if (vkAllocateCommandBuffers(
			m_device.GetLogicalDevice(),
			&allocInfo,
			allocatedCommandBuffers.data()) != VK_SUCCESS
		) {
			throw std::runtime_error("failed to allocate command buffer!");
		}


		for (auto allocatedCommandBuffer : allocatedCommandBuffers)
		{
			output.emplace_back(
				m_commandBuffers.emplace_back(CommandBuffer{
					allocatedCommandBuffer
				})
			);
		}

		return output;
	}

	VkCommandPool CommandPool::GetHandle() const
	{
		return m_handle;
	}
}
