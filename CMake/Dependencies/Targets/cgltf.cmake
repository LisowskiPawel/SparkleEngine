# cgltf - Single-header glTF 2.0 parser
# https://github.com/jkuhlmann/cgltf
#
# Header-only library. Define CGLTF_IMPLEMENTATION in exactly one .cpp file
# before including cgltf.h to generate the implementation.
#
# Target:  cgltf (INTERFACE)
# Usage:   target_link_libraries(YourTarget PRIVATE cgltf)
sparkle_declare_git_dependency(cgltf
    https://github.com/jkuhlmann/cgltf.git
    360db1a95480fe102ae9c69b27c5d101167ff5ba)
FetchContent_Populate(cgltf)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

# Silence MSVC CRT deprecation warnings in cgltf implementation code.
if(MSVC)
    target_compile_definitions(cgltf INTERFACE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(cgltf INTERFACE /wd4996)
endif()
