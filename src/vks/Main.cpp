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
#include <vks/sync/Fence.h>
#include <vks/sync/Semaphore.h>
#include <vks/GraphicsPipeline.h>
#include <vks/utils/ShaderUtils.h>
#include <vks/utils/DeviceManager.h>
#include <vks/utils/CommandBufferUtils.h>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
	// TODO: Double check we are not duplicating the shader program, but passing a ref
	vks::ShaderProgram program(std::to_array({
		vertexStage,
		fragmentStage
	}));

	// Doesn't change even when the swap chain is recreated
	vks::utils::SwapChainOptimalConfig swapChainOptimalConfig = vks::utils::SwapChainUtils::CalculateSwapChainOptimalConfig(
		device.GetSwapChainSupportDetails()
	);

	std::unique_ptr<vks::RenderPass> renderPass = std::make_unique<vks::RenderPass>(
		device.GetLogicalDevice(),
		swapChainOptimalConfig.surfaceFormat.format
	);

	std::unique_ptr<vks::GraphicsPipeline> graphicsPipeline = std::make_unique<vks::GraphicsPipeline>(
		device.GetLogicalDevice(),
		vks::GraphicsPipelineDesc{
			.program = program,
			.renderPass = *renderPass
		}
	);

	// Can be outside of the render loop, since the window is marked as non-resizable.
	// If this were to change, this should be evaluated each frame, and the swap chain would
	// need to be recreated.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D extent = vks::utils::SwapChainUtils::CalculateSwapExtent(
		swapChainOptimalConfig.capabilities,
		VkExtent2D{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		}
	);

	std::unique_ptr<vks::SwapChain> swapChain = std::make_unique<vks::SwapChain>(
		device,
		surface->GetHandle(),
		extent,
		swapChainOptimalConfig
	);

	std::vector<vks::Framebuffer> framebuffers = swapChain->CreateFramebuffers(renderPass->GetHandle());

	const uint8_t k_maxFramesInFlight = 2;
	uint8_t currentFrameIndex = 0;

	std::unique_ptr<vks::CommandPool> commandPool = std::make_unique<vks::CommandPool>(device);
	std::vector<std::reference_wrapper<vks::CommandBuffer>> commandBuffers = commandPool->AllocateCommandBuffers(k_maxFramesInFlight);

	struct FrameSyncObjects
	{
		std::unique_ptr<vks::sync::Semaphore> imageAvailableSemaphore;
		std::unique_ptr<vks::sync::Semaphore> renderFinishedSemaphore;
		std::unique_ptr<vks::sync::Fence> inFlightFence;
	};

	std::array<FrameSyncObjects, k_maxFramesInFlight> frameSyncObjectsArray;
	for (uint8_t i = 0; i < k_maxFramesInFlight; ++i)
	{
		frameSyncObjectsArray[i].imageAvailableSemaphore = std::make_unique<vks::sync::Semaphore>(device.GetLogicalDevice());
		frameSyncObjectsArray[i].renderFinishedSemaphore = std::make_unique<vks::sync::Semaphore>(device.GetLogicalDevice());
		frameSyncObjectsArray[i].inFlightFence = std::make_unique<vks::sync::Fence>(device.GetLogicalDevice(), true);
	}

	uint32_t swapImageIndex = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		vks::CommandBuffer& commandBuffer = commandBuffers[currentFrameIndex].get();
		FrameSyncObjects& frameSyncObjects = frameSyncObjectsArray[currentFrameIndex];

		device.WaitForFences({ *frameSyncObjects.inFlightFence });

		try
		{
			uint32_t swapImageIndex = swapChain->AcquireNextImage(*frameSyncObjects.imageAvailableSemaphore);
		}
		catch (vks::OutOfDateSwapChain)
		{
			int width = 0, height = 0;
			glfwGetFramebufferSize(window, &width, &height);

			// To handle when the window is minimized, since 0 isn't a valid size for a swap chain
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(window, &width, &height);
				glfwWaitEvents();
			}

			VkExtent2D newExtent = vks::utils::SwapChainUtils::CalculateSwapExtent(
				swapChainOptimalConfig.capabilities,
				VkExtent2D{
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				}
			);

			device.WaitIdle();

			framebuffers.clear();
			swapChain.reset();

			// Recreate the swapchain
			swapChain = std::make_unique<vks::SwapChain>(
				device,
				surface->GetHandle(),
				newExtent,
				swapChainOptimalConfig
			);

			framebuffers = swapChain->CreateFramebuffers(renderPass->GetHandle());
			continue;
		}

		device.ResetFences({ *frameSyncObjects.inFlightFence });

		commandBuffer.Reset();
		commandBuffer.Begin();

		commandBuffer.BeginRenderPass(
			renderPass->GetHandle(),
			framebuffers[swapImageIndex].GetHandle(),
			swapChain->GetExtent()
		);

		commandBuffer.BindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			graphicsPipeline->GetHandle()
		);

		// As noted in the fixed functions chapter, we did specify viewport and scissor state for this pipeline to be dynamic.
		// So we need to set them in the command buffer before issuing our draw command:
		commandBuffer.SetViewport({
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(swapChain->GetExtent().width),
			.height = static_cast<float>(swapChain->GetExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		});

		commandBuffer.SetScissor({
			.offset = { 0, 0 },
			.extent = swapChain->GetExtent()
		});

		commandBuffer.Draw(3, 1);

		commandBuffer.EndRenderPass();
		commandBuffer.End();

		device.GetGraphicsQueue().Submit(
			{ commandBuffer },
			{ *frameSyncObjects.imageAvailableSemaphore },
			{ *frameSyncObjects.renderFinishedSemaphore },
			*frameSyncObjects.inFlightFence
		);

		device.GetPresentQueue().Present(
			{ *frameSyncObjects.renderFinishedSemaphore },
			*swapChain,
			swapImageIndex
		);

		currentFrameIndex = (currentFrameIndex + 1) % k_maxFramesInFlight;
	}

	// Operations in drawFrame are asynchronous. That means that when we exit the loop in mainLoop,
	// drawing and presentation operations may still be going on.
	// Cleaning up resources while that is happening is a bad idea.
	// To fix that problem, we should wait for the logical device to finish operations before exiting mainLoop and destroying the window.
	device.WaitIdle();

	for (uint8_t i = 0; i < k_maxFramesInFlight; ++i)
	{
		frameSyncObjectsArray[i].imageAvailableSemaphore.reset();
		frameSyncObjectsArray[i].renderFinishedSemaphore.reset();
		frameSyncObjectsArray[i].inFlightFence.reset();
	}

	commandPool.reset();
	framebuffers.clear();
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
