# Assimp - Open Asset Import Library
# https://github.com/assimp/assimp
#
# SourceImporters owns FBX and PLY import; runtime modules remain cooked-only.
set(_sparkle_assimp_revision c35200e38ea8f058812b83de2ef32c6093b0ece2)
sparkle_declare_git_dependency(assimp
    https://github.com/assimp/assimp.git
    ${_sparkle_assimp_revision})
FetchContent_Populate(assimp)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_PLY_IMPORTER ON CACHE BOOL "" FORCE)
# Pinned Assimp uses the legacy POST_BUILD default; make that supported policy explicit.
set(_sparkle_previous_cmp0175_default "${CMAKE_POLICY_DEFAULT_CMP0175}")
set(CMAKE_POLICY_DEFAULT_CMP0175 OLD)
sparkle_add_dependency_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
set(CMAKE_POLICY_DEFAULT_CMP0175 "${_sparkle_previous_cmp0175_default}")

if(TARGET assimp AND NOT TARGET assimp::assimp)
    add_library(assimp::assimp ALIAS assimp)
endif()

if(TARGET assimp)
    if(MSVC)
        target_compile_options(assimp PRIVATE /MP1 /FS)
    endif()
    set_target_properties(assimp PROPERTIES FOLDER "ThirdParty/Assimp")
    sparkle_set_source_package(assimp "Assimp" "${_sparkle_assimp_revision}"
        "BSD-3-Clause" "https://github.com/assimp/assimp")
endif()
