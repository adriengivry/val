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

	void* windowHandle = nullptr;
	void* instanceHandle = nullptr;

#if defined(_WIN32) || defined(_WIN64)
	windowHandle = glfwGetWin32Window(window);
	instanceHandle = GetModuleHandle(nullptr);
#endif

	assert(windowHandle != nullptr && "Invalid window handle");

	std::unique_ptr<vks::Instance> instance = std::make_unique<vks::Instance>(
		vks::InstanceDesc{
			.requiredExtensions = requiredExtensions,
			.surfaceDesc = vks::SurfaceDesc{
				.windowHandle = windowHandle,
				.instanceHandle = instanceHandle
			}
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

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	instance.reset();

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}
