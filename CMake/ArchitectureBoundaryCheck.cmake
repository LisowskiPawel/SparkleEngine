cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED SPARKLE_REPO_ROOT)
    get_filename_component(SPARKLE_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

file(TO_CMAKE_PATH "${SPARKLE_REPO_ROOT}" SPARKLE_REPO_ROOT)

set(SPARKLE_BOUNDARY_SOURCE_FILE_REGEX "\\.(c|cc|cpp|cppm|cxx|h|hh|hpp|hxx|inl|ixx|cmake|manifest)$|/CMakeLists\\.txt$")
set(SPARKLE_BOUNDARY_NATIVE_API_REGEX "<d3d12\\.h>|<vulkan/vulkan\\.h>|ID3D12|D3D12_|Vk[A-Z]|vk[A-Z]|Vulkan::Vulkan|\"D3D12/|\"Vulkan/")
set(SPARKLE_BOUNDARY_NATIVE_PTLAS_REGEX "VK_NV_partitioned_acceleration_structure|VkPartitionedAccelerationStructure|VkBuildPartitionedAccelerationStructure|VK_DESCRIPTOR_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_NV|vk(Get|Cmd)PartitionedAccelerationStructures|NvAPI_D3D12|NVAPI_D3D12|D3D12_RTAS_PARTITIONED_TLAS|ExecuteIndirectRTASOperations")
set(SPARKLE_BOUNDARY_RENDERER_SHADER_DATA_REGEX "(Frame|View|ViewCamera|ViewTemporal)UniformData|PerObjectConstantBufferData|SceneLightingUniformData|LightGpuData|MeshInstanceShaderData|MeshInstanceData|VertexSkinInfluenceData|JointMatrixData")
set(SPARKLE_BOUNDARY_D3D12_IN_VULKAN_REGEX "D3D12/|<d3d12\\.h>|ID3D12|D3D12_")
set(SPARKLE_BOUNDARY_VULKAN_IN_D3D12_REGEX "Vulkan/|<vulkan/vulkan\\.h>|Vk[A-Z]|vk[A-Z]|Vulkan::Vulkan")
set(SPARKLE_BOUNDARY_RENDERER_HIGH_LEVEL_ORCHESTRATOR_REGEX
    "^Engine/Renderer/Private/(Host/RendererHost|Frame/FramePipeline|Providers/RendererImageProviderStack)\\.(cpp|h)$")
set(SPARKLE_BOUNDARY_RENDERER_PROVIDER_DETAIL_REGEX
    "Streamline/|Upscaling/Nvidia|RayReconstruction/Nvidia|NvidiaDlss")
set(SPARKLE_BOUNDARY_RENDERER_VENDOR_INTEROP_REGEX
    "^Engine/Renderer/Private/(Streamline|Upscaling/NvidiaDlss|RayReconstruction/NvidiaDlssRayReconstruction)/")
set(SPARKLE_BOUNDARY_RENDERER_PRIVATE_ONLY_PUBLIC_PATH_REGEX
    "^Engine/Renderer/Public/(Debug|FrameGraph|SceneData|ShaderParameters)/|^Engine/Renderer/Public/Meshes/GpuMeshHandle\\.h$|^Engine/Renderer/Public/Resources/Textures/DefaultTextures\\.h$")
set(SPARKLE_BOUNDARY_NATIVE_DPI_POLICY_REGEX
    "Set(Process|Thread)DpiAwareness|GetDpiFor(Window|Monitor|System)|DPI_AWARENESS_CONTEXT_|VS_DPI_AWARE|<dpiAware(ness)?|ImGui_ImplWin32_(EnableDpiAwareness|GetDpiScaleFor)|DisplaySize[ \t]*=[ \t]*ImVec2")

set_property(GLOBAL PROPERTY SPARKLE_BOUNDARY_FAILURES "")

include("${CMAKE_CURRENT_LIST_DIR}/Architecture/ModuleBoundaries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/LoggingBoundaries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/ToolBoundaries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/RhiBoundaries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/RendererContracts.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/RendererComposition.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Architecture/SourceScan.cmake")

message(STATUS "Sparkle architecture boundary check")
message(STATUS "Repository root: ${SPARKLE_REPO_ROOT}")

sparkle_boundary_collect_source_files(
    SPARKLE_BOUNDARY_SOURCE_FILES
    "CMake"
    "Engine"
    "Tools"
    "Projects")
list(APPEND SPARKLE_BOUNDARY_SOURCE_FILES "${SPARKLE_REPO_ROOT}/CMakeLists.txt")

foreach(_file IN LISTS SPARKLE_BOUNDARY_SOURCE_FILES)
    sparkle_boundary_scan_file("${_file}")
endforeach()

get_property(_failures GLOBAL PROPERTY SPARKLE_BOUNDARY_FAILURES)
if(_failures)
    list(LENGTH _failures _failure_count)
    message(STATUS "Architecture boundary violations:")
    foreach(_failure IN LISTS _failures)
        message(STATUS "  - ${_failure}")
    endforeach()
    message(FATAL_ERROR "Architecture boundary check failed with ${_failure_count} violation(s).")
endif()

message(STATUS "Architecture boundary check passed with no new violations.")
