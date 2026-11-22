set(SPARKLE_SHADERMAKE_REVISION 4f1ffd56dac27fc3d620ad7efeee42b4dd71fa72)
set(_shadermake_patch "${CMAKE_CURRENT_LIST_DIR}/Patches/ShaderMakeCompilerContract.patch")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_shadermake_patch}")
file(SHA256 "${_shadermake_patch}" _shadermake_patch_hash)

sparkle_declare_git_dependency(shadermake
    https://github.com/NVIDIA-RTX/ShaderMake.git
    ${SPARKLE_SHADERMAKE_REVISION}
    ID shadermake VERSION "git ${SPARKLE_SHADERMAKE_REVISION} + patch-sha256:${_shadermake_patch_hash}" PACKAGE_NAME "ShaderMake" LICENSE_ID "MIT"
    REQUIRED_PATHS ShaderMake/CMakeLists.txt ShaderMake/ShaderMake.cpp ShaderMake/ShaderBlob.cpp ShaderMake/ShaderBlob.h ShaderMake/argparse.c ShaderMake/argparse.h LICENSE.txt ThirdPartyLicenses.txt)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

# Patch a private build copy; the shared source cache stays at its clean pin.
sparkle_prepare_patched_dependency_files("${shadermake_SOURCE_DIR}" "${shadermake_BINARY_DIR}"
    "${_shadermake_patch}" _shadermake_patched ShaderMake/ShaderMake.cpp)

# The leaf project builds the tool without entering upstream SDK discovery/downloads.
sparkle_add_dependency_subdirectory("${shadermake_SOURCE_DIR}/ShaderMake"
    "${shadermake_BINARY_DIR}/tool" EXCLUDE_FROM_ALL)
set_property(TARGET ShaderMake PROPERTY SOURCES
    "${_shadermake_patched}/ShaderMake/ShaderMake.cpp"
    "${shadermake_SOURCE_DIR}/ShaderMake/ShaderBlob.cpp"
    "${shadermake_SOURCE_DIR}/ShaderMake/argparse.c")
target_include_directories(ShaderMake PRIVATE "${shadermake_SOURCE_DIR}/ShaderMake")
set_target_properties(ShaderMake PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON FOLDER "ThirdParty/shadermake")

sparkle_add_dependency_source_package(ShaderMake shadermake)
sparkle_add_bundled_source_package(ShaderMake shadermake "ShaderMake bundled argparse" MIT
    ShaderMake/argparse.c ShaderMake/argparse.h ThirdPartyLicenses.txt)
