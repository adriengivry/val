/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <vks/SwapChain.h>
#include <vks/Framebuffer.h>
#include <vks/CommandBuffer.h>

namespace vks
{
	struct DescriptorSetLayoutBinding
	{
		VkDescriptorType type;
		VkShaderStageFlags stageFlags;
		uint32_t binding;
	};

	class DescriptorSetLayout
	{
	public:
		/**
		* Creates a descriptor set layout
		*/
		DescriptorSetLayout(VkDevice p_device, std::span<const DescriptorSetLayoutBinding> p_bindings);

		/**
		* Destroys the descriptor set layout
		*/
		virtual ~DescriptorSetLayout();

		/**
		* Returns the underlying VkDescriptorSetLayout handle
		*/
		VkDescriptorSetLayout GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
	};
}
