# SPIRV-Reflect - SPIR-V reflection (Khronos)
# https://github.com/KhronosGroup/SPIRV-Reflect
#
# Used by Tools/Shaders/ShaderCompiler/Private/Backend/Dxc to extract reflection from the
# SPIR-V blobs DXC emits in `-spirv` mode. Single .c + headers; we build it
# as a small static library so its symbols stay out of every translation
# unit that includes the header.
#
# Target:  spirv_reflect (STATIC)
# Usage:   target_link_libraries(YourTarget PRIVATE spirv_reflect)
#          #include <spirv_reflect.h>
sparkle_declare_git_dependency(spirv_reflect
    https://github.com/KhronosGroup/SPIRV-Reflect.git
    b4dc70d8e6ac30c719a2d05b8ad05e1d277c92b4)
sparkle_log_dependency_step("SPIRV-Reflect" "vulkan-sdk-1.3.290.0" "~2 MB" "SPIR-V reflection for offline shader compiler backends" "https://github.com/KhronosGroup/SPIRV-Reflect.git")
FetchContent_Populate(spirv_reflect)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("SPIRV-Reflect" "${spirv_reflect_SOURCE_DIR}" "~2 MB")
    return()
endif()

add_library(spirv_reflect STATIC
    ${spirv_reflect_SOURCE_DIR}/spirv_reflect.c
    ${spirv_reflect_SOURCE_DIR}/spirv_reflect.h
)

target_include_directories(spirv_reflect PUBLIC
    ${spirv_reflect_SOURCE_DIR}
    ${spirv_reflect_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(spirv_reflect PRIVATE /W0)
endif()

set_target_properties(spirv_reflect PROPERTIES FOLDER "ThirdParty")
sparkle_log_dependency_ready("SPIRV-Reflect" "${spirv_reflect_SOURCE_DIR}" "~2 MB")
