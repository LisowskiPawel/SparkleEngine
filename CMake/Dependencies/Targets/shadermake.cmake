sparkle_declare_git_dependency(shadermake
    https://github.com/NVIDIA-RTX/ShaderMake.git
    ${SPARKLE_SHADERMAKE_REVISION})
FetchContent_Populate(shadermake)
sparkle_prepare_git_fetchcontent_source(shadermake
    EXPECTED_REVISION "${SPARKLE_SHADERMAKE_REVISION}"
    REQUIRED_PATHS ShaderMake/CMakeLists.txt ShaderMake/ShaderMake.cpp
        ShaderMake/ShaderBlob.cpp ShaderMake/ShaderBlob.h
        ShaderMake/argparse.c ShaderMake/argparse.h LICENSE.txt ThirdPartyLicenses.txt)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

# Patch a private build copy; the shared source cache stays at its clean pin.
set(_shadermake_patch "${CMAKE_CURRENT_LIST_DIR}/../Patches/ShaderMakeCompilerContract.patch")
set(_shadermake_patched "${shadermake_BINARY_DIR}/compiler-contract")
configure_file("${shadermake_SOURCE_DIR}/ShaderMake/ShaderMake.cpp"
    "${_shadermake_patched}/input/ShaderMake/ShaderMake.cpp" COPYONLY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_shadermake_patch}")
execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${shadermake_SOURCE_DIR}" rev-parse --absolute-git-dir
    OUTPUT_VARIABLE _shadermake_git_directory OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND "${GIT_EXECUTABLE}" "--git-dir=${_shadermake_git_directory}"
        "--work-tree=${_shadermake_patched}/input" apply "${_shadermake_patch}"
    WORKING_DIRECTORY "${_shadermake_patched}/input"
    RESULT_VARIABLE _shadermake_patch_result ERROR_VARIABLE _shadermake_patch_error)
if(NOT _shadermake_patch_result EQUAL 0)
    message(FATAL_ERROR "ShaderMake compiler contract patch failed: ${_shadermake_patch_error}")
endif()
configure_file("${_shadermake_patched}/input/ShaderMake/ShaderMake.cpp"
    "${_shadermake_patched}/ShaderMake.cpp" COPYONLY)

# The leaf project builds the tool without entering upstream SDK discovery/downloads.
sparkle_add_dependency_subdirectory("${shadermake_SOURCE_DIR}/ShaderMake"
    "${shadermake_BINARY_DIR}/tool" EXCLUDE_FROM_ALL)
set_property(TARGET ShaderMake PROPERTY SOURCES
    "${_shadermake_patched}/ShaderMake.cpp"
    "${shadermake_SOURCE_DIR}/ShaderMake/ShaderBlob.cpp"
    "${shadermake_SOURCE_DIR}/ShaderMake/argparse.c")
target_include_directories(ShaderMake PRIVATE "${shadermake_SOURCE_DIR}/ShaderMake")
set_target_properties(ShaderMake PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON FOLDER "ThirdParty")
sparkle_set_source_package(ShaderMake "ShaderMake" "git ${SPARKLE_SHADERMAKE_REVISION} + compiler-contract"
    "MIT" "https://github.com/NVIDIA-RTX/ShaderMake/tree/${SPARKLE_SHADERMAKE_REVISION}")
