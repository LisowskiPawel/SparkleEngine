# Third-party integrations

[Integration contract](../Docs/ThirdPartyMigration.md#ownership-and-dependency-workflow) | [Primary precedents](../Docs/References.md#integration-precedents)

Start with the dependency's `Dependency.cmake`: it owns identity, required inputs, build options, targets and package facts. Optional `Bindings/` contains Sparkle-authored compile carriers/configuration; `Patches/` contains maintained upstream corrections. Fetched vendor source remains in ignored `build/_deps`, and each configured tree owns its compiled and patched outputs. The checked-in D3DX12 header lives separately under `D3DX12/Include`, beside its license and origin record.

Shared acquisition, admission and cache locking live in [CMake/Dependencies](../CMake/Dependencies); qualified GPU-crash SDK inputs belong to [Diagnostics](../CMake/Diagnostics/GpuCrashInputs.cmake). Engine/tool `Private` folders own semantic adapters and runtime lifetime; their `CMake` folders select backends/installed SDKs and stage the payload they consume. Qt/MSVC delivery and installed capture/compiler SDKs retain those owners. Slang's [patches](slang/Patches) support the installed SDK selected by [ShaderCompiler](../Tools/Shaders/ShaderCompiler/CMake/ShaderCompilerSdks.cmake); they do not introduce a second acquisition path.

| Consumer or capability | Dependency recipes |
| --- | --- |
| Core logging, JSON, processes and math | [spdlog](spdlog/Dependency.cmake), [nlohmann/json](nlohmann_json/Dependency.cmake), [Boost.Process](boost-process/Dependency.cmake), [DirectXMath](directxmath/Dependency.cmake) |
| Host/tool command lines | [CLI11](cli11/Dependency.cmake) |
| Task execution | [enkiTS target composition](enkits/Dependency.cmake), [qualified source materialization/admission](enkits/RuntimeSource.cmake) |
| Platform windows/input | [SDL3](sdl3/Dependency.cmake) |
| Editor and runtime UI | [ImGui](imgui/Dependency.cmake), selected [graphics backends](imgui/Backends.cmake), [Font Awesome](editor-icons/Dependency.cmake) |
| Source import | [cgltf](cgltf/Dependency.cmake), [Assimp](assimp/Dependency.cmake), [MikkTSpace](mikktspace/Dependency.cmake) |
| Texture processing and compression | [DirectXTex](directxtex/Dependency.cmake), [stb](stb/Dependency.cmake), [TinyEXR](tinyexr/Dependency.cmake), [KTX](ktx/Dependency.cmake), [zlib](zlib/Dependency.cmake) |
| Animation runtime and offline builders | [ozz-animation](ozz_animation/Dependency.cmake) |
| Shader tools | [ShaderMake](shadermake/Dependency.cmake), [SPIRV-Reflect](spirv-reflect/Dependency.cmake) |
| D3D12/Vulkan allocation and API helpers | [D3D12MA](d3d12ma/Dependency.cmake), [VMA](vma/Dependency.cmake), [D3DX12](D3DX12/Dependency.cmake), [NVAPI](nvidia-nvapi/Dependency.cmake) |
| Renderer image providers | [Streamline](nvidia-streamline/Dependency.cmake) |
| Native capture/markers | [WinPixEventRuntime](winpixeventruntime/Dependency.cmake) |

Update an integration through its recipe and actual consumers. Preserve its source/patch identity, ABI, notices and runtime closure; remove it with the last consumer. Bindings and patches remain first-party maintenance wherever placed. Pins and package facts stay in executable declarations rather than a second version catalogue.
