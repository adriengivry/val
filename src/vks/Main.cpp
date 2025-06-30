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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
#include <vks/Buffer.h>
#include <vks/DescriptorSetLayout.h>
#include <vks/DescriptorPool.h>
#include <vks/DescriptorSet.h>
#include <vks/sync/Fence.h>
#include <vks/sync/Semaphore.h>
#include <vks/GraphicsPipeline.h>
#include <vks/utils/ShaderUtils.h>
#include <vks/utils/DeviceManager.h>

namespace
{
	struct FrameData
	{
		vks::CommandBuffer& commandBuffer;
		vks::Framebuffer& framebuffer;
		vks::Buffer& ubo;
		vks::DescriptorSet& descriptorSet;
		std::unique_ptr<vks::sync::Semaphore> imageAvailableSemaphore;
		std::unique_ptr<vks::sync::Semaphore> renderFinishedSemaphore;
		std::unique_ptr<vks::sync::Fence> inFlightFence;
	};

	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
	};

	template<class T>
	struct VertexInputDescription
	{
		static auto GetBindingDescription();
		static auto GetAttributeDescriptions();
	};

	template<>
	struct VertexInputDescription<Vertex>
	{
		static auto GetBindingDescription()
		{
			return std::to_array<VkVertexInputBindingDescription>({
				{
					.binding = 0,
					.stride = sizeof(Vertex),
					.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				}
			});
		}

		static auto GetAttributeDescriptions()
		{
			return std::to_array<VkVertexInputAttributeDescription>({
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(Vertex, pos)
				},
				{
					.location = 1,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(Vertex, color)
				}
			});
		}
	};

	constexpr auto k_vertices = std::to_array<Vertex>({
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	});

	constexpr auto k_indices = std::to_array<uint32_t>({
		0, 1, 2,
		2, 3, 0
	});

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	VkExtent2D GetWindowSize(GLFWwindow* p_window)
	{
		int width, height;
		glfwGetFramebufferSize(p_window, &width, &height);
		return {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
	}

	std::vector<std::string> GetGlfwRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<std::string> requiredExtensions;
		requiredExtensions.reserve(glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			requiredExtensions.push_back(glfwExtensions[i]);
		}

		return requiredExtensions;
	}

	vks::SurfaceDesc GetGlfwSurfaceDesc(GLFWwindow* p_window)
	{
#if defined(_WIN32) || defined(_WIN64)
		return {
			.windowHandle = glfwGetWin32Window(p_window),
			.instanceHandle = GetModuleHandle(nullptr)
		};
#else
#error Only supporting Windows for now!
#endif
	}
}

int RunVulkan(GLFWwindow* window)
{
	// Create instance
	auto instance = std::make_unique<vks::Instance>(
		vks::InstanceDesc{
			.requiredExtensions = GetGlfwRequiredExtensions()
		}
	);

	// Create surface
	auto surface = std::make_unique<vks::Surface>(
		instance->GetHandle(),
		GetGlfwSurfaceDesc(window)
	);

	assert((surface && surface->GetHandle() != VK_NULL_HANDLE) && "invalid surface handle, cannot continue since headless isn't supported");

	// Create device manager so we can find a suitable device
	auto deviceManager = std::make_unique<vks::utils::DeviceManager>(
		instance->GetHandle(),
		surface->GetHandle()
	);

	// Retrieve the most suited device
	vks::Device& device = deviceManager->GetSuitableDevice();
	device.CreateLogicalDevice(instance->GetValidationLayers());

	// Create vertex module
	auto vertexModule = std::make_unique<vks::ShaderModule>(
		device.GetLogicalDevice(),
		vks::utils::ShaderUtils::ReadShaderFile("assets/shaders/foo.vert.spv")
	);

	// Create fragment module
	auto fragmentModule = std::make_unique<vks::ShaderModule>(
		device.GetLogicalDevice(),
		vks::utils::ShaderUtils::ReadShaderFile("assets/shaders/foo.frag.spv")
	);

	// Create shader program
	vks::ShaderStage vertexStage(*vertexModule, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	vks::ShaderStage fragmentStage(*fragmentModule, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	vks::ShaderProgram program({
		vertexStage,
		fragmentStage
	});

	auto hostVertexBuffer = std::make_unique<vks::Buffer>(
		device,
		vks::BufferDesc{
			.size = sizeof(k_vertices),
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		}
	);
	hostVertexBuffer->Allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	hostVertexBuffer->Upload(k_vertices.data());

	std::unique_ptr<vks::Buffer> hostIndexBuffer = std::make_unique<vks::Buffer>(
		device,
		vks::BufferDesc{
			.size = sizeof(k_indices),
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		}
	);
	hostIndexBuffer->Allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	hostIndexBuffer->Upload(k_indices.data());

	std::unique_ptr<vks::Buffer> deviceVertexBuffer = std::make_unique<vks::Buffer>(
		device,
		vks::BufferDesc{
			.size = sizeof(k_vertices),
			.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		}
	);
	deviceVertexBuffer->Allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	auto deviceIndexBuffer = std::make_unique<vks::Buffer>(
		device,
		vks::BufferDesc{
			.size = sizeof(k_indices),
			.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		}
	);
	deviceIndexBuffer->Allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	auto descriptorSetLayout = std::make_unique<vks::DescriptorSetLayout>(
		device.GetLogicalDevice(),
		std::to_array<vks::DescriptorSetLayoutBinding>({
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.binding = 0
			}
		})
	);

	// Doesn't change even when the swap chain is recreated
	vks::utils::SwapChainOptimalConfig swapChainOptimalConfig = vks::utils::SwapChainUtils::CalculateSwapChainOptimalConfig(
		device.GetSwapChainSupportDetails(),
		GetWindowSize(window)
	);

	auto renderPass = std::make_unique<vks::RenderPass>(
		device.GetLogicalDevice(),
		swapChainOptimalConfig.surfaceFormat.format
	);

	auto graphicsPipeline = std::make_unique<vks::GraphicsPipeline>(
		device.GetLogicalDevice(),
		vks::GraphicsPipelineDesc{
			.program = program,
			.renderPass = *renderPass,
			.vertexInputAttributeDesc = VertexInputDescription<Vertex>::GetAttributeDescriptions(),
			.vertexInputBindingDesc = VertexInputDescription<Vertex>::GetBindingDescription(),
			.descriptorSetLayouts = std::to_array({std::ref(*descriptorSetLayout)})
		}
	);

	auto swapChain = std::make_unique<vks::SwapChain>(
		device,
		surface->GetHandle(),
		swapChainOptimalConfig
	);

	auto framebuffers = swapChain->CreateFramebuffers(renderPass->GetHandle());

	const uint8_t k_maxFramesInFlight = 2;
	uint8_t currentFrameIndex = 0;

	std::vector<vks::Buffer> ubos;
	ubos.reserve(k_maxFramesInFlight);
	for (uint8_t i = 0; i < k_maxFramesInFlight; ++i)
	{
		auto& ubo = ubos.emplace_back(
			device,
			vks::BufferDesc{
				.size = sizeof(UniformBufferObject),
				.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			}
		);

		ubo.Allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	auto descriptorPool = std::make_unique<vks::DescriptorPool>(device);

	std::vector<std::reference_wrapper<vks::DescriptorSet>> descriptorSets =
		descriptorPool->AllocateDescriptorSets(
			*descriptorSetLayout,
			k_maxFramesInFlight
		);

	for (size_t i = 0; i < k_maxFramesInFlight; i++)
	{
		descriptorSets[i].get().Write(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			std::to_array({ std::ref(ubos[i]) })
		);
	}

	auto commandPool = std::make_unique<vks::CommandPool>(device);
	std::vector<std::reference_wrapper<vks::CommandBuffer>> transferCommandBuffers = commandPool->AllocateCommandBuffers(1);
	std::vector<std::reference_wrapper<vks::CommandBuffer>> commandBuffers = commandPool->AllocateCommandBuffers(k_maxFramesInFlight);

	vks::CommandBuffer& transferCommandBuffer = transferCommandBuffers.front().get();

	// Upload CPU (host) buffers to the GPU (device)
	transferCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	transferCommandBuffer.CopyBuffer(*hostVertexBuffer, *deviceVertexBuffer);
	transferCommandBuffer.CopyBuffer(*hostIndexBuffer, *deviceIndexBuffer);
	transferCommandBuffer.End();
	device.GetGraphicsQueue().Submit({ transferCommandBuffer });
	device.WaitIdle();

	// At this point we don't need the client copy of these buffers anymore
	hostVertexBuffer->Deallocate();
	hostIndexBuffer->Deallocate();

	std::vector<FrameData> frameDataArray;
	frameDataArray.reserve(k_maxFramesInFlight);
	for (uint8_t i = 0; i < k_maxFramesInFlight; ++i)
	{
		frameDataArray.emplace_back(
			commandBuffers[i],
			framebuffers[i],
			ubos[i],
			descriptorSets[i],
			std::make_unique<vks::sync::Semaphore>(device.GetLogicalDevice()),
			std::make_unique<vks::sync::Semaphore>(device.GetLogicalDevice()),
			std::make_unique<vks::sync::Fence>(device.GetLogicalDevice(), true)
		);
	}

	uint32_t swapImageIndex = 0;

	auto recreateSwapChain = [&] {
		VkExtent2D windowSize{ 0, 0 };

		// To handle when the window is minimized, since 0 isn't a valid size for a swap chain
		while (windowSize.width == 0 || windowSize.height == 0)
		{
			windowSize = GetWindowSize(window);
			glfwWaitEvents();
		}
		
		device.QuerySwapChainDetails();

		swapChainOptimalConfig = vks::utils::SwapChainUtils::CalculateSwapChainOptimalConfig(
			device.GetSwapChainSupportDetails(),
			windowSize
		);

		device.WaitIdle();

		framebuffers.clear();
		swapChain.reset();

		// Recreate the swapchain
		swapChain = std::make_unique<vks::SwapChain>(
			device,
			surface->GetHandle(),
			swapChainOptimalConfig
		);

		framebuffers = swapChain->CreateFramebuffers(renderPass->GetHandle());
	};

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		vks::CommandBuffer& commandBuffer = commandBuffers[currentFrameIndex].get();
		FrameData& frameData = frameDataArray[currentFrameIndex];

		device.WaitForFences({ *frameData.inFlightFence });

		try
		{
			uint32_t swapImageIndex = swapChain->AcquireNextImage(*frameData.imageAvailableSemaphore);
		}
		catch (vks::OutOfDateSwapChain)
		{
			recreateSwapChain();
			continue;
		}

		device.ResetFences({ *frameData.inFlightFence });

		const float time = static_cast<float>(glfwGetTime());

		// Update UBO data each frame
		UniformBufferObject uboData{
			.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			.proj = glm::perspective(glm::radians(45.0f), swapChainOptimalConfig.extent.width / (float)swapChainOptimalConfig.extent.height, 0.1f, 10.0f)
		};

		// GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
		// The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
		// If you don't do this, then the image will be rendered upside down.
		uboData.proj[1][1] *= -1;

		frameData.ubo.Upload(&uboData);

		commandBuffer.Reset();
		commandBuffer.Begin();

		commandBuffer.BeginRenderPass(
			renderPass->GetHandle(),
			frameData.framebuffer.GetHandle(),
			swapChain->GetDesc().extent
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
			.width = static_cast<float>(swapChain->GetDesc().extent.width),
			.height = static_cast<float>(swapChain->GetDesc().extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		});

		commandBuffer.SetScissor({
			.offset = { 0, 0 },
			.extent = swapChain->GetDesc().extent
		});

		commandBuffer.BindVertexBuffers(
			std::to_array({ std::ref(*deviceVertexBuffer) }),
			std::to_array<uint64_t>({0})
		);

		commandBuffer.BindIndexBuffer(
			*deviceIndexBuffer
		);

		commandBuffer.BindDescriptorSets(
			std::to_array({ std::ref(frameData.descriptorSet) }),
			graphicsPipeline->GetLayout()
		);

		commandBuffer.DrawIndexed(static_cast<uint32_t>(k_indices.size()));

		commandBuffer.EndRenderPass();
		commandBuffer.End();

		device.GetGraphicsQueue().Submit(
			{ commandBuffer },
			{ *frameData.imageAvailableSemaphore },
			{ *frameData.renderFinishedSemaphore },
			* frameData.inFlightFence
		);

		try
		{
			device.GetPresentQueue().Present(
				{ *frameData.renderFinishedSemaphore },
				*swapChain,
				swapImageIndex
			);
		}
		catch (vks::OutOfDateSwapChain)
		{
			recreateSwapChain();
			continue;
		}

		currentFrameIndex = (currentFrameIndex + 1) % k_maxFramesInFlight;
	}

	// Operations in drawFrame are asynchronous. That means that when we exit the loop in mainLoop,
	// drawing and presentation operations may still be going on.
	// Cleaning up resources while that is happening is a bad idea.
	// To fix that problem, we should wait for the logical device to finish operations before exiting mainLoop and destroying the window.
	device.WaitIdle();

	return EXIT_SUCCESS;
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "vulkan-sandbox", nullptr, nullptr);

	int exitCode = RunVulkan(window);

	glfwDestroyWindow(window);
	glfwTerminate();

	return exitCode;
}
