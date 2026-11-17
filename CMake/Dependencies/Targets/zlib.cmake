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
    ${_sparkle_zlib_revision})
sparkle_log_dependency_step("zlib" "v1.3.1" "~1 MB" "Compression backend used by TinyEXR and Assimp" "https://github.com/madler/zlib.git")
FetchContent_Populate(zlib)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("zlib" "${zlib_SOURCE_DIR}" "~1 MB")
    return()
endif()

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
sparkle_add_dependency_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} EXCLUDE_FROM_ALL)
set_property(TARGET zlibstatic PROPERTY SPARKLE_SOURCE_PACKAGE
    "{\"name\":\"zlib\",\"versionInfo\":\"1.3.1\",\"licenseDeclared\":\"Zlib\",\"downloadLocation\":\"https://github.com/madler/zlib/tree/${_sparkle_zlib_revision}\"}")

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

sparkle_log_dependency_ready("zlib" "${zlib_SOURCE_DIR}" "~1 MB")
