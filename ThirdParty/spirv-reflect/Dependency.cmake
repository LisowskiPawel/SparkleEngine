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
    b4dc70d8e6ac30c719a2d05b8ad05e1d277c92b4
    ID spirv-reflect VERSION "b4dc70d8e6ac30c719a2d05b8ad05e1d277c92b4" PACKAGE_NAME "SPIRV-Reflect" LICENSE_ID "Apache-2.0"
    REQUIRED_PATHS spirv_reflect.c spirv_reflect.h include/spirv/unified1/spirv.h LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(spirv_reflect STATIC
    ${spirv_reflect_SOURCE_DIR}/spirv_reflect.c
    ${spirv_reflect_SOURCE_DIR}/spirv_reflect.h
)

target_include_directories(spirv_reflect SYSTEM PUBLIC
    ${spirv_reflect_SOURCE_DIR}
    ${spirv_reflect_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(spirv_reflect PRIVATE /W0)
endif()

set_target_properties(spirv_reflect PROPERTIES FOLDER "ThirdParty/spirv-reflect")

sparkle_add_dependency_source_package(spirv_reflect spirv-reflect)
sparkle_add_bundled_source_package(spirv_reflect spirv-reflect "SPIRV-Reflect bundled SPIRV-Headers" Apache-2.0
    include/spirv/unified1/spirv.h LICENSE)
