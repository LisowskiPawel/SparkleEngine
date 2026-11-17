sparkle_declare_git_dependency(sparkle_vma
    https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    009ecd192c1289c7529bff248a16cfe896254816)

FetchContent_GetProperties(sparkle_vma)
if(NOT sparkle_vma_POPULATED)
    FetchContent_Populate(sparkle_vma)
endif()

add_library(SparkleVMA INTERFACE)
target_include_directories(SparkleVMA
    INTERFACE
        ${sparkle_vma_SOURCE_DIR}/include
)
target_link_libraries(SparkleVMA
    INTERFACE
        Vulkan::Vulkan
)
set_target_properties(SparkleVMA PROPERTIES FOLDER "ThirdParty")

file(GLOB_RECURSE SPARKLE_RHI_VULKAN_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Vulkan/*.cpp
)

file(GLOB_RECURSE SPARKLE_RHI_VULKAN_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Vulkan/*.h
)

if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    list(FILTER SPARKLE_RHI_VULKAN_SOURCES EXCLUDE REGEX "/Diagnostics/GpuCrash/")
    list(FILTER SPARKLE_RHI_VULKAN_HEADERS EXCLUDE REGEX "/Diagnostics/GpuCrash/")
endif()

set(SPARKLE_RHI_VULKAN_THIRD_PARTY_SOURCES
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

add_library(SparkleRHI_Vulkan STATIC
    ${SPARKLE_RHI_VULKAN_SOURCES}
    ${SPARKLE_RHI_VULKAN_HEADERS}
    ${SPARKLE_RHI_VULKAN_THIRD_PARTY_SOURCES}
)

sparkle_configure_rhi_implementation_target(SparkleRHI_Vulkan)

target_link_libraries(SparkleRHI_Vulkan
    PRIVATE
        SparkleCore
        imgui
    PRIVATE
        SparklePlatform
        SparkleRHIDiagnostics
        Vulkan::Vulkan
        SparkleVMA
)

sparkle_target_precompile_headers(SparkleRHI_Vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Vulkan/VulkanPCH.h
)

list(APPEND SPARKLE_RHI_BACKEND_TARGETS SparkleRHI_Vulkan)
