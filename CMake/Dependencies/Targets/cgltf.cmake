# cgltf - Single-header glTF 2.0 parser
# https://github.com/jkuhlmann/cgltf
#
# Target:  cgltf (STATIC); the implementation is independent of importer edits.
# Usage:   target_link_libraries(YourTarget PRIVATE cgltf)
set(_sparkle_cgltf_revision 360db1a95480fe102ae9c69b27c5d101167ff5ba)
set(_cgltf_patch "${CMAKE_CURRENT_LIST_DIR}/../Patches/CgltfAccessorBounds.patch")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_cgltf_patch}")
file(SHA256 "${_cgltf_patch}" _cgltf_patch_hash)
sparkle_declare_git_dependency(cgltf
    https://github.com/jkuhlmann/cgltf.git
    ${_sparkle_cgltf_revision}
    ID cgltf VERSION "1.15 + patch-sha256:${_cgltf_patch_hash}" PACKAGE_NAME "cgltf" LICENSE_ID "MIT"
    REQUIRED_PATHS cgltf.h LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(cgltf STATIC "${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/Bindings/cgltf/CgltfImplementation.cpp")
sparkle_prepare_patched_dependency_files("${cgltf_SOURCE_DIR}" "${cgltf_BINARY_DIR}" "${_cgltf_patch}" _cgltf_build_source cgltf.h)
target_include_directories(cgltf SYSTEM PUBLIC "${_cgltf_build_source}")
set_target_properties(cgltf PROPERTIES FOLDER "ThirdParty/cgltf")

sparkle_add_dependency_source_package(cgltf cgltf)

# Silence MSVC CRT deprecation warnings in cgltf implementation code.
if(MSVC)
    target_compile_definitions(cgltf PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(cgltf PRIVATE /W0)
endif()
