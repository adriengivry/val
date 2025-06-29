/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/Buffer.h>
#include <vks/Device.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace
{
	uint32_t FindMemoryType(VkPhysicalDevice p_physicalDevice, uint32_t p_typeFilter, VkMemoryPropertyFlags p_properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(p_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((p_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & p_properties) == p_properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
}

namespace vks
{
	Buffer::Buffer(Device& p_device, const BufferDesc& p_desc) :
		m_device(p_device)
	{
		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = p_desc.size,
			.usage = p_desc.usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};

		if (vkCreateBuffer(
			m_device.GetLogicalDevice(),
			&bufferInfo,
			nullptr,
			&m_handle
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}
	}

	Buffer::~Buffer()
	{
		if (IsAllocated())
		{
			Deallocate();
		}

		vkDestroyBuffer(m_device.GetLogicalDevice(), m_handle, nullptr);
	}

	bool Buffer::IsAllocated() const
	{
		return
			m_memory != VK_NULL_HANDLE &&
			m_allocatedBytes > 0;
	}

	void Buffer::Allocate(VkMemoryPropertyFlags p_properties)
	{
		assert(!IsAllocated());

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device.GetLogicalDevice(), m_handle, &memRequirements);

		const auto memoryType = FindMemoryType(
			m_device.GetPhysicalDevice(),
			memRequirements.memoryTypeBits,
			p_properties
		);

		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = memoryType
		};

		if (vkAllocateMemory(
			m_device.GetLogicalDevice(),
			&allocInfo,
			nullptr,
			&m_memory
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_device.GetLogicalDevice(), m_handle, m_memory, 0);

		m_allocatedBytes = memRequirements.size;
	}

	void Buffer::Deallocate()
	{
		assert(IsAllocated());

		vkFreeMemory(m_device.GetLogicalDevice(), m_memory, nullptr);
		m_memory = VK_NULL_HANDLE;
		m_allocatedBytes = 0;
	}

	void Buffer::Upload(const void* p_data, std::optional<BufferMemoryRange> p_memoryRange)
	{
		assert(IsAllocated());
		assert(!p_memoryRange.has_value() || p_memoryRange->offset + p_memoryRange->size <= m_allocatedBytes); // out-of-bounds check

		const uint64_t offset = p_memoryRange.has_value() ? p_memoryRange->offset : 0;
		const uint64_t size = p_memoryRange.has_value() ? p_memoryRange->size : m_allocatedBytes;

		void* destPtr;
		VkResult result = vkMapMemory(
			m_device.GetLogicalDevice(),
			m_memory,
			offset,
			size,
			0,
			&destPtr
		);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to map buffer memory!");
		}

		std::memcpy(destPtr, p_data, size);

		vkUnmapMemory(m_device.GetLogicalDevice(), m_memory);
	}

	uint64_t Buffer::GetAllocatedBytes() const
	{
		return m_allocatedBytes;
	}

	VkBuffer Buffer::GetHandle() const
	{
		return m_handle;
	}
}
