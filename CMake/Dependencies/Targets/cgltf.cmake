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
sparkle_log_dependency_step("cgltf" "v1.15" "~1 MB" "Single-header glTF 2.0 parser for source scene imports" "https://github.com/jkuhlmann/cgltf.git")
FetchContent_Populate(cgltf)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("cgltf" "${cgltf_SOURCE_DIR}" "~1 MB")
    return()
endif()

add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

# Silence MSVC CRT deprecation warnings in cgltf implementation code.
if(MSVC)
    target_compile_definitions(cgltf INTERFACE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(cgltf INTERFACE /wd4996)
endif()

sparkle_log_dependency_ready("cgltf" "${cgltf_SOURCE_DIR}" "~1 MB")
