sparkle_declare_git_dependency(sparkle_vma
    https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    009ecd192c1289c7529bff248a16cfe896254816
    ID vma VERSION "009ecd192c1289c7529bff248a16cfe896254816" PACKAGE_NAME "VulkanMemoryAllocator" LICENSE_ID MIT
    REQUIRED_PATHS include/vk_mem_alloc.h LICENSE.txt)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

sparkle_add_source_package(Vulkan::Vulkan Vulkan-Headers "${Vulkan_VERSION}"
    Apache-2.0 "https://github.com/KhronosGroup/Vulkan-Headers/tree/v${Vulkan_VERSION}")

add_library(SparkleVMA STATIC "${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/Bindings/vma/VmaImplementation.cpp")
target_include_directories(SparkleVMA
    SYSTEM PUBLIC
        ${sparkle_vma_SOURCE_DIR}/include
)
target_link_libraries(SparkleVMA
    PUBLIC
        Vulkan::Vulkan
)
target_compile_definitions(SparkleVMA PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN VK_USE_PLATFORM_WIN32_KHR=1)
if(MSVC)
    target_compile_options(SparkleVMA PRIVATE /W0)
endif()
set_target_properties(SparkleVMA PROPERTIES FOLDER "ThirdParty/VMA")
sparkle_add_dependency_source_package(SparkleVMA vma)

