MS Visual Studio 2019 Solution for x64 (debug+release, no 32 bit).

Required dependencies (may be installed via [vcpkg tool](https://github.com/microsoft/vcpkg)):

- stb (2020-09-14)
- tinyobjloader (2.0.0-rc2)
- glfw3 (3.3.4)
- glm (0.9.9.8)

Version numbers indicate last version that verified ok.

Of course, we also require the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
It is located via VULKAN_SDK env var set by SDK installer and must be downloaded and
installed manually (last verified ok: 1.2.176.1).

To start each example via F5 shortcut, set Startup Project in Solution Properties
to "Current selection".

If you want a working syntax highlighting for the shader sources, this is what worked for me
at the time of writing this:

- https://github.com/danielscherzer/GLSL for MSVC 2019 (may cause problems with nsight or syntax highlight extensions like "Syntax Highlighting Pack").
- https://github.com/stef-levesque/vscode-shader/ for *VS Code*.
