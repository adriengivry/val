/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if defined(_WIN32) || defined(_WIN64)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <cassert>
#include <iostream>
#include <vector>
#include <span>
#include <ranges>

#include <vks/Instance.h>
#include <vks/Surface.h>
#include <vks/SwapChain.h>
#include <vks/ShaderModule.h>
#include <vks/ShaderStage.h>
#include <vks/ShaderProgram.h>
#include <vks/RenderPass.h>
#include <vks/Framebuffer.h>
#include <vks/CommandPool.h>
#include <vks/GraphicsPipeline.h>
#include <vks/utils/ShaderUtils.h>
#include <vks/utils/DeviceManager.h>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "vulkan-sandbox", nullptr, nullptr);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<std::string> requiredExtensions;
	requiredExtensions.reserve(glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
	{
		requiredExtensions.push_back(glfwExtensions[i]);
	}

	std::unique_ptr<vks::Instance> instance = std::make_unique<vks::Instance>(
		vks::InstanceDesc{
			.requiredExtensions = requiredExtensions
		}
	);

	// Instead of handling the surface creation inside of vks::Instance, we could create a platform-agnostic vulkan instead just like that:
	/*
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
	*/
	// But since we dont want to assume that the user is using GLFW, it might be better to have an actual code path to create a surface for
	// each supported platform.
	std::unique_ptr<vks::Surface> surface = std::make_unique<vks::Surface>(
		instance->GetHandle(),
#if defined(_WIN32) || defined(_WIN64)
		vks::SurfaceDesc{
			.windowHandle = glfwGetWin32Window(window),
			.instanceHandle = GetModuleHandle(nullptr)
#else
#error Only supporting Windows for now!
#endif
	});

	assert((surface && surface->GetHandle() != VK_NULL_HANDLE) && "invalid surface handle, cannot continue since headless isn't supported");

	std::unique_ptr<vks::utils::DeviceManager> deviceManager = std::make_unique<vks::utils::DeviceManager>(
		instance->GetHandle(),
		surface->GetHandle()
	);

	vks::Device& device = deviceManager->GetSuitableDevice();
	device.CreateLogicalDevice(instance->GetValidationLayers());

	// Can be outside of the render loop, since the window is marked as non-resizable.
	// If this were to change, this should be evaluated each frame, and the swap chain would
	// need to be recreated.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	vks::utils::SwapChainDesc swapChainDesc = vks::utils::SwapChainUtils::CreateSwapChainDesc(
		device.GetSwapChainSupportDetails(),
		VkExtent2D{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		}
	);

	std::unique_ptr<vks::SwapChain> swapChain = std::make_unique<vks::SwapChain>(
		device,
		surface->GetHandle(),
		swapChainDesc
	);

	std::unique_ptr<vks::ShaderModule> vertexModule = std::make_unique<vks::ShaderModule>(
		device.GetLogicalDevice(),
		vks::utils::ShaderUtils::ReadShaderFile("assets/shaders/foo.vert.spv")
	);

	std::unique_ptr<vks::ShaderModule> fragmentModule = std::make_unique<vks::ShaderModule>(
		device.GetLogicalDevice(),
		vks::utils::ShaderUtils::ReadShaderFile("assets/shaders/foo.frag.spv")
	);

	vks::ShaderStage vertexStage(*vertexModule, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	vks::ShaderStage fragmentStage(*fragmentModule, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	vks::ShaderProgram program(std::to_array({
		vertexStage,
		fragmentStage
	}));

	std::unique_ptr<vks::RenderPass> renderPass = std::make_unique<vks::RenderPass>(
		device.GetLogicalDevice(),
		vks::RenderPassDesc{
			.swapChain = *swapChain
		}
	);

	std::unique_ptr<vks::GraphicsPipeline> graphicsPipeline = std::make_unique<vks::GraphicsPipeline>(
		device.GetLogicalDevice(),
		vks::GraphicsPipelineDesc{
			.program = program,
			.swapChain = *swapChain,
			.renderPass = *renderPass
		}
	);

	// Retrieve swap chain images and use them for rendering operations
	const auto& swapChainImages = swapChain->GetImages();

	std::vector<VkImageView> swapChainImageViews(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapChainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swapChainDesc.surfaceFormat.format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		if (vkCreateImageView(
			device.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&swapChainImageViews[i]) != VK_SUCCESS
			) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	std::vector<vks::Framebuffer> swapChainFramebuffers;
	swapChainFramebuffers.reserve(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		swapChainFramebuffers.emplace_back(
			device.GetLogicalDevice(),
			vks::FramebufferDesc{
				.attachments = std::to_array({ swapChainImageViews[i] }),
				.swapChain = *swapChain,
				.renderPass = renderPass->GetHandle()
			}
		);
	}

	std::unique_ptr<vks::CommandPool> commandPool = std::make_unique<vks::CommandPool>(device);
	vks::CommandBuffer& commandBuffer = commandPool->AllocateCommandBuffer();

	while (!glfwWindowShouldClose(window))
	{
		// Draw things here

		glfwPollEvents();
	}

	for (auto imageView : swapChainImageViews)
	{
		vkDestroyImageView(device.GetLogicalDevice(), imageView, nullptr);
	}

	commandPool.reset();
	swapChainFramebuffers.clear();
	renderPass.reset();
	graphicsPipeline.reset();
	fragmentModule.reset();
	vertexModule.reset();
	swapChain.reset();
	deviceManager.reset();
	surface.reset();
	instance.reset();

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}
