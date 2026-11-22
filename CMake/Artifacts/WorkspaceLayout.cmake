# Sparkle development artifact naming contract.
#
# Generated build trees are private CMake/MSBuild state. Runnable development
# products live under artifacts/.

if(NOT DEFINED SPARKLE_REPOSITORY_ROOT)
    set(SPARKLE_REPOSITORY_ROOT "${CMAKE_SOURCE_DIR}")
endif()
get_filename_component(SPARKLE_REPOSITORY_ROOT "${SPARKLE_REPOSITORY_ROOT}" ABSOLUTE)
include("${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/SourceCacheAccess.cmake")

set(_sparkle_canonical_build_root "${SPARKLE_REPOSITORY_ROOT}/build")
get_filename_component(_sparkle_canonical_build_root "${_sparkle_canonical_build_root}" ABSOLUTE)
get_filename_component(_sparkle_configured_build_root "${CMAKE_BINARY_DIR}" ABSOLUTE)
set(SPARKLE_ARTIFACT_VARIANT "" CACHE STRING "Artifact namespace for an isolated build/variants/<name> workspace.")

file(RELATIVE_PATH
    _sparkle_build_root_relative_to_canonical
    "${_sparkle_canonical_build_root}"
    "${_sparkle_configured_build_root}")

if(IS_ABSOLUTE "${_sparkle_build_root_relative_to_canonical}"
   OR _sparkle_build_root_relative_to_canonical MATCHES "^\\.\\.([/\\\\]|$)")
    message(FATAL_ERROR
        "Sparkle build trees must stay under '${_sparkle_canonical_build_root}'. "
        "Use SparkleLauncher for the canonical workspace or configure an isolated validation tree as "
        "'build/variants/<name>' with '-DSPARKLE_ARTIFACT_VARIANT=<name>'. "
        "Rejected build tree: '${_sparkle_configured_build_root}'.")
endif()

set(_sparkle_artifact_root "${SPARKLE_REPOSITORY_ROOT}/artifacts")
set(FETCHCONTENT_BASE_DIR "${_sparkle_canonical_build_root}/_deps" CACHE PATH
    "Shared Sparkle source-dependency cache; fixed beneath the canonical build root." FORCE)

if(SPARKLE_ARTIFACT_VARIANT AND NOT SPARKLE_ARTIFACT_VARIANT MATCHES "^[A-Za-z0-9._-]+$")
    message(FATAL_ERROR "SPARKLE_ARTIFACT_VARIANT contains unsupported path characters: '${SPARKLE_ARTIFACT_VARIANT}'")
endif()

set(_sparkle_dependency_sync_build_tree FALSE)
set(_sparkle_launcher_product_build_tree FALSE)
get_filename_component(_sparkle_launcher_source_root "${SPARKLE_REPOSITORY_ROOT}/Tools/Launcher" ABSOLUTE)
get_filename_component(_sparkle_configured_source_root "${CMAKE_SOURCE_DIR}" ABSOLUTE)
if(_sparkle_configured_source_root STREQUAL _sparkle_launcher_source_root)
    set(_sparkle_expected_launcher_build_tree "private/tools/SparkleLauncher")
    if(NOT _sparkle_build_root_relative_to_canonical STREQUAL _sparkle_expected_launcher_build_tree)
        message(FATAL_ERROR
            "The Launcher product entry point must use exactly 'build/${_sparkle_expected_launcher_build_tree}'. "
            "Rejected build tree: '${_sparkle_configured_build_root}'.")
    endif()
    if(SPARKLE_ARTIFACT_VARIANT)
        message(FATAL_ERROR "The Launcher product entry point publishes the canonical Launcher artifact, not a variant.")
    endif()
    set(_sparkle_launcher_product_build_tree TRUE)
endif()

if(DEFINED SPARKLE_SYNC_SOURCE_DEPENDENCY AND NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY MATCHES "^[A-Za-z0-9._-]+$")
        message(FATAL_ERROR
            "SPARKLE_SYNC_SOURCE_DEPENDENCY contains unsupported path characters: '${SPARKLE_SYNC_SOURCE_DEPENDENCY}'")
    endif()

    set(_sparkle_expected_dependency_sync_tree
        "_dependency-sync/${SPARKLE_SYNC_SOURCE_DEPENDENCY}")
    if(NOT _sparkle_build_root_relative_to_canonical STREQUAL _sparkle_expected_dependency_sync_tree)
        message(FATAL_ERROR
            "Source dependency sync must use exactly 'build/${_sparkle_expected_dependency_sync_tree}'. "
            "Rejected build tree: '${_sparkle_configured_build_root}'.")
    endif()
    if(SPARKLE_ARTIFACT_VARIANT)
        message(FATAL_ERROR "Source dependency sync does not publish an artifact variant.")
    endif()
    set(_sparkle_dependency_sync_build_tree TRUE)
endif()

if(_sparkle_dependency_sync_build_tree)
    # Selective dependency sync exits before product targets are generated. Its
    # CMake state remains private while fetched sources use build/_deps.
elseif(_sparkle_launcher_product_build_tree)
    # The Launcher has an owned product entry point and private build tree.
elseif(_sparkle_configured_build_root STREQUAL _sparkle_canonical_build_root)
    if(SPARKLE_ARTIFACT_VARIANT)
        message(FATAL_ERROR
            "The canonical 'build/' tree must publish to canonical 'artifacts/'. "
            "SPARKLE_ARTIFACT_VARIANT is valid only with its matching 'build/variants/<name>' tree.")
    endif()
else()
    if(NOT _sparkle_build_root_relative_to_canonical MATCHES "^variants[/\\\\][A-Za-z0-9._-]+$")
        message(FATAL_ERROR
            "Alternate Sparkle build trees must use exactly 'build/variants/<name>'. "
            "Rejected build tree: '${_sparkle_configured_build_root}'.")
    endif()

    get_filename_component(_sparkle_build_variant_name "${_sparkle_configured_build_root}" NAME)
    if(NOT SPARKLE_ARTIFACT_VARIANT STREQUAL _sparkle_build_variant_name)
        message(FATAL_ERROR
            "Alternate Sparkle build tree '${_sparkle_configured_build_root}' must publish to its matching isolated artifact namespace. "
            "Pass '-DSPARKLE_ARTIFACT_VARIANT=${_sparkle_build_variant_name}'.")
    endif()
endif()

if(SPARKLE_ARTIFACT_VARIANT)
    set(_sparkle_active_artifact_root "${_sparkle_artifact_root}/${SPARKLE_ARTIFACT_VARIANT}")
else()
    set(_sparkle_active_artifact_root "${_sparkle_artifact_root}")
endif()

set(_sparkle_product_root "${_sparkle_active_artifact_root}/dev")
set(_sparkle_tool_artifact_root "${_sparkle_product_root}/Tools")
set(_sparkle_project_artifact_root "${_sparkle_product_root}/Projects")
set(_sparkle_runtime_support_artifact_root "${_sparkle_product_root}/Engine/Binaries")
set(_sparkle_library_artifact_root "${_sparkle_product_root}/libraries")
set(_sparkle_symbol_artifact_root "${_sparkle_active_artifact_root}/symbols")
