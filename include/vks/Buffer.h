/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace vks
{
	class Device;

	struct BufferDesc
	{
		uint64_t size;
		VkBufferUsageFlags usage;
	};

	struct BufferMemoryRange
	{
		uint64_t offset;
		uint64_t size;
	};

	class Buffer
	{
	public:
		/**
		* Creates a buffer
		*/
		Buffer(Device& p_device, const BufferDesc& p_desc);

		/**
		* Destroys the buffer
		*/
		virtual ~Buffer();

		/**
		* Returns true if the buffer is allocated 
		*/
		bool IsAllocated() const;

		/**
		* Allocate memory for the buffer 
		*/
		void Allocate();

		/**
		* Deallocates memory for the buffer
		*/
		void Deallocate();

		/**
		* Uploads data to the allocated memory
		*/
		void Upload(const void* p_data, std::optional<BufferMemoryRange> p_memoryRange = std::nullopt);

		/**
		* Returns the underlying VkBuffer handle
		*/
		VkBuffer GetHandle() const;

	private:
		Device& m_device;
		VkBuffer m_handle = VK_NULL_HANDLE;
		VkDeviceMemory m_memory = VK_NULL_HANDLE;
		uint64_t m_allocatedBytes = 0;
	};
}
