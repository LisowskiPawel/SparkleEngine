include("${SPARKLE_REPOSITORY_ROOT}/ThirdParty/vma/Dependency.cmake")

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

add_library(SparkleRHI_Vulkan STATIC
    ${SPARKLE_RHI_VULKAN_SOURCES}
    ${SPARKLE_RHI_VULKAN_HEADERS}
)

sparkle_configure_rhi_implementation_target(SparkleRHI_Vulkan)

target_link_libraries(SparkleRHI_Vulkan
    PRIVATE
        SparkleCore
        imgui
        imgui_vulkan
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
