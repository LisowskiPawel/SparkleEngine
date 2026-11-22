# Assimp - Open Asset Import Library
# https://github.com/assimp/assimp
#
# SourceImporters owns FBX and PLY import; runtime modules remain cooked-only.
set(_sparkle_assimp_revision c35200e38ea8f058812b83de2ef32c6093b0ece2)
set(_assimp_patch "${CMAKE_CURRENT_LIST_DIR}/Patches/AssimpMaterialKeyBounds.patch")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_assimp_patch}")
file(SHA256 "${_assimp_patch}" _assimp_patch_hash)
sparkle_declare_git_dependency(assimp
    https://github.com/assimp/assimp.git
    ${_sparkle_assimp_revision}
    ID assimp VERSION "5.4.3 + patch-sha256:${_assimp_patch_hash}" PACKAGE_NAME "Assimp" LICENSE_ID "BSD-3-Clause"
    REQUIRED_PATHS CMakeLists.txt LICENSE include/assimp/Importer.hpp contrib/poly2tri/LICENSE contrib/utf8cpp/doc/LICENSE contrib/openddlparser/LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_assimp)
    # Preserve the admitted DLL contract without publishing a generic cache option.
    set(BUILD_SHARED_LIBS ON)
    if(NOT TARGET ZLIB::ZLIB)
        message(FATAL_ERROR "Assimp requires Sparkle's admitted external zlib target.")
    endif()
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    set(ASSIMP_INSTALL OFF)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF)
    set(ASSIMP_NO_EXPORT ON)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
    set(ASSIMP_BUILD_FBX_IMPORTER ON)
    set(ASSIMP_BUILD_PLY_IMPORTER ON)
    # Pinned Assimp uses the legacy POST_BUILD default; make that supported policy explicit.
    set(_sparkle_previous_cmp0175_default "${CMAKE_POLICY_DEFAULT_CMP0175}")
    set(CMAKE_POLICY_DEFAULT_CMP0175 OLD)
    sparkle_add_dependency_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
    set(CMAKE_POLICY_DEFAULT_CMP0175 "${_sparkle_previous_cmp0175_default}")

    if(TARGET assimp AND NOT TARGET assimp::assimp)
        add_library(assimp::assimp ALIAS assimp)
    endif()
endfunction()
sparkle_add_assimp()

if(TARGET assimp)
    sparkle_prepare_patched_dependency_files("${assimp_SOURCE_DIR}" "${assimp_BINARY_DIR}" "${_assimp_patch}"
        _assimp_build_source code/Material/MaterialSystem.cpp)
    get_target_property(_assimp_sources assimp SOURCES)
    set(_material_source_count 0)
    foreach(source IN LISTS _assimp_sources)
        if(source MATCHES "(^|/)Material/MaterialSystem[.]cpp$")
            list(REMOVE_ITEM _assimp_sources "${source}")
            math(EXPR _material_source_count "${_material_source_count} + 1")
        endif()
    endforeach()
    if(NOT _material_source_count EQUAL 1)
        message(FATAL_ERROR "Assimp's material source membership changed; review the bounds patch before building.")
    endif()
    list(APPEND _assimp_sources "${_assimp_build_source}/code/Material/MaterialSystem.cpp")
    set_property(TARGET assimp PROPERTY SOURCES "${_assimp_sources}")
    set_property(SOURCE "${_assimp_build_source}/code/Material/MaterialSystem.cpp" TARGET_DIRECTORY assimp
        APPEND PROPERTY INCLUDE_DIRECTORIES "${assimp_SOURCE_DIR}/code/Material")
    if(MSVC)
        target_compile_options(assimp PRIVATE /MP1 /FS)
    endif()
    set_target_properties(assimp PROPERTIES FOLDER "ThirdParty/assimp")

    sparkle_add_dependency_source_package(assimp assimp)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled OpenDDL" MIT contrib/openddlparser/LICENSE)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled poly2tri" BSD-3-Clause contrib/poly2tri/LICENSE)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled utf8cpp" BSL-1.0 contrib/utf8cpp/doc/LICENSE)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled Clipper" BSL-1.0 contrib/clipper/License.txt)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled Open3DGC" MIT contrib/Open3DGC/o3dgcCommon.h)
    sparkle_add_bundled_source_package(assimp assimp "Assimp bundled minizip" "Zlib AND Info-ZIP" contrib/unzip/unzip.c contrib/unzip/crypt.h)
endif()
