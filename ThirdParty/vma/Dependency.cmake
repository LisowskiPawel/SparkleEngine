set(_vma_patch "${CMAKE_CURRENT_LIST_DIR}/Patches/VmaCopyMappingLifetime.patch")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_vma_patch}")
file(SHA256 "${_vma_patch}" _vma_patch_hash)
sparkle_declare_git_dependency(sparkle_vma
    https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    009ecd192c1289c7529bff248a16cfe896254816
    ID vma VERSION "009ecd192c1289c7529bff248a16cfe896254816 + patch-sha256:${_vma_patch_hash}" PACKAGE_NAME "VulkanMemoryAllocator" LICENSE_ID MIT
    REQUIRED_PATHS include/vk_mem_alloc.h LICENSE.txt)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

sparkle_add_source_package(Vulkan::Vulkan Vulkan-Headers "${Vulkan_VERSION}"
    Apache-2.0 "https://github.com/KhronosGroup/Vulkan-Headers/tree/v${Vulkan_VERSION}")

add_library(SparkleVMA STATIC "${CMAKE_CURRENT_LIST_DIR}/Bindings/VmaImplementation.cpp")
target_compile_features(SparkleVMA PRIVATE cxx_std_14)
sparkle_prepare_patched_dependency_files("${sparkle_vma_SOURCE_DIR}" "${sparkle_vma_BINARY_DIR}"
    "${_vma_patch}" _vma_patched include/vk_mem_alloc.h)
target_include_directories(SparkleVMA
    SYSTEM PUBLIC
        ${_vma_patched}/include
)
target_link_libraries(SparkleVMA
    PUBLIC
        Vulkan::Vulkan
)
target_compile_definitions(SparkleVMA PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN VK_USE_PLATFORM_WIN32_KHR=1)
if(MSVC)
    target_compile_options(SparkleVMA PRIVATE /W0)
endif()
set_target_properties(SparkleVMA PROPERTIES FOLDER "ThirdParty/vma")
sparkle_add_dependency_source_package(SparkleVMA vma)

