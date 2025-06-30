# Vulkan Abstraction Layer

**Vulkan Abstraction Layer** — *val* — is a lightweight, permissively licensed (MIT), cross-platform abstraction layer for the Vulkan graphics API, written in C++20.

## Disclaimer
*val* is a work in progress, certainly not production-ready, and may contain bugs or limitations.

## Motivation

The goal of this project is to strengthen my proficiency with Vulkan and ultimately integrate Vulkan support into [OpenRHI](https://github.com/adriengivry/orhi). Modern graphics APIs like Vulkan, DirectX 12, and Metal differ significantly from legacy APIs such as OpenGL. To make informed decisions about which abstractions should be exposed through [OpenRHI](https://github.com/adriengivry/orhi)'s interface, it's essential to develop a deep understanding of these modern paradigms. Originally designed around OpenGL, [OpenRHI](https://github.com/adriengivry/orhi) currently lacks support for several key concepts fundamental to modern APIs, including command buffers, device selection, descriptor sets, and synchronization mechanisms.

## Why is this on GitHub?

I believe this project holds significant educational value, especially for those new to Vulkan. It offers a clear and practical introduction to the structure of the Vulkan API and how its core concepts interact. Additionally, it can serve as a solid starting point for rendering engine projects that intend to use Vulkan exclusively. For engines that aim to support multiple graphics backends, however, I recommend exploring [OpenRHI](https://github.com/adriengivry/orhi) instead, as it provides a more flexible, backend-agnostic abstraction layer.

## References
Some references I used to get started with Vulkan and to build this project:
- [Vulkan Tutorial](https://vulkan-tutorial.com)
- [Vulkan Documentation](https://docs.vulkan.org)
- [Vulkan Specs](https://registry.khronos.org/vulkan/specs/latest/man/html/)

## Dependencies
To compile *val*, you'll need the Vulkan SDK installed on your machine. When using premake5, *val* relies on the `VK_SDK_PATH` environment variable to locate the Vulkan SDK. If this environment variable isn't defined, you'll need to set it manually.

## Quick Start (Examples)
> ⚠️ You'll need to clone *val*, including its submodules (`--recurse-submodules`), to run the examples!

You can try out *val* in just a few clicks on Windows by using the built-in project generation script. It uses `premake5.exe` under the hood, which is bundled with this repository. If you'd like to use *val* on a different platform, I suggest downloading [premake5](https://premake.github.io/download) from its official website, installing it, and running it from the `examples/` folder.

### Windows (Visual Studio 2022)
```powershell
git clone https://github.com/adriengivry/val --recurse-submodules
cd .\val\
.\gen_examples.bat
start .\examples\val-examples.sln
```

## Other Platforms
1. Get [premake5](https://premake.github.io/download).
2. Install premake or copy the executable's path.
3. Navigate to the `examples/` folder.
4. Run `premake5` with the desired IDE name (e.g., vs2022, gmake, xcode4, etc.).

## Using *val* in Your Application
Since *val* is set up to use premake5, the easiest way to add *val* to your project is to use premake5 for your project as well.

### Premake5
1. Add *val* as a submodule to your project:
```powershell
git submodule add https://github.com/adriengivry/val
```
2. Modify your premake5 workspace configuration to include:
```lua
workspace "your-workspace-name"
    include "path/to/val"
```
3. Modify your premake5 project configuration to include:
```lua
project "your-project-name"
    includedirs {
        "path/to/val/include",
    }

    links {
        "val"
    }
```

### CMake
If you'd prefer to use CMake, you'll need to add a custom `CMakeLists.txt` file. Don't worry, *val* is **extremely** easy to set up.
