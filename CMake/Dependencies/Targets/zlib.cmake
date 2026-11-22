# zlib - Compression backend for TinyEXR and Assimp
# https://github.com/madler/zlib
#
# Assimp's bundled zlib is 1.2.13 and still uses K&R-style function
# definitions that clang-cl warns about as non-prototype definitions.
# Keep zlib as an explicit pinned dependency so Assimp links the modernized
# 1.3.1 source instead of building its bundled copy.
#
# Target:  zlibstatic
# Usage:   target_link_libraries(YourTarget PRIVATE zlibstatic)
set(_sparkle_zlib_revision 51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf)
sparkle_declare_git_dependency(zlib
    https://github.com/madler/zlib.git
    ${_sparkle_zlib_revision}
    ID zlib VERSION "1.3.1" PACKAGE_NAME "zlib" LICENSE_ID "Zlib"
    REQUIRED_PATHS CMakeLists.txt zlib.h LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

# This upstream CMake renames its source zconf.h. Configure unchanged tracked
# bytes in this tree's private copy, omitting that unused pregenerated header.
set(_zlib_build_source "${zlib_BINARY_DIR}/source-${_sparkle_zlib_revision}")
execute_process(COMMAND "${GIT_EXECUTABLE}" ls-files
    WORKING_DIRECTORY "${zlib_SOURCE_DIR}" OUTPUT_VARIABLE _zlib_files
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
string(REPLACE "\n" ";" _zlib_files "${_zlib_files}")
foreach(file IN LISTS _zlib_files)
    if(NOT file STREQUAL zconf.h)
        configure_file("${zlib_SOURCE_DIR}/${file}" "${_zlib_build_source}/${file}" COPYONLY)
    endif()
endforeach()
block(SCOPE_FOR VARIABLES)
    set(ZLIB_BUILD_EXAMPLES OFF)
    sparkle_add_dependency_subdirectory("${_zlib_build_source}" "${zlib_BINARY_DIR}" EXCLUDE_FROM_ALL)
endblock()

sparkle_add_dependency_source_package(zlibstatic zlib)

if(TARGET zlibstatic AND NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

if(TARGET zlib)
    set_target_properties(zlib PROPERTIES FOLDER "ThirdParty/zlib")
endif()

if(TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "ThirdParty/zlib")
endif()

set(ZLIB_FOUND TRUE)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
