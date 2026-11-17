# Sparkle development artifact naming contract.
#
# Generated build trees are private CMake/MSBuild state. Runnable development
# products live under artifacts/.

if(NOT DEFINED SPARKLE_REPOSITORY_ROOT)
    set(SPARKLE_REPOSITORY_ROOT "${CMAKE_SOURCE_DIR}")
endif()
get_filename_component(SPARKLE_REPOSITORY_ROOT "${SPARKLE_REPOSITORY_ROOT}" ABSOLUTE)

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

set(_sparkle_legacy_generated_paths
    "${SPARKLE_REPOSITORY_ROOT}/Saved"
    "${SPARKLE_REPOSITORY_ROOT}/imgui.ini"
    "${SPARKLE_REPOSITORY_ROOT}/CMakeCache.txt"
    "${SPARKLE_REPOSITORY_ROOT}/CMakeFiles"
    "${SPARKLE_REPOSITORY_ROOT}/bin"
    "${SPARKLE_REPOSITORY_ROOT}/obj"
    "${SPARKLE_REPOSITORY_ROOT}/bld"
    "${SPARKLE_REPOSITORY_ROOT}/Debug"
    "${SPARKLE_REPOSITORY_ROOT}/DebugPublic"
    "${SPARKLE_REPOSITORY_ROOT}/Release"
    "${SPARKLE_REPOSITORY_ROOT}/Releases"
    "${SPARKLE_REPOSITORY_ROOT}/x64"
    "${SPARKLE_REPOSITORY_ROOT}/x86"
    "${SPARKLE_REPOSITORY_ROOT}/Intermediate"
    "${SPARKLE_REPOSITORY_ROOT}/DerivedDataCache"
    "${SPARKLE_REPOSITORY_ROOT}/Temp"
    "${SPARKLE_REPOSITORY_ROOT}/Tmp"
    "${SPARKLE_REPOSITORY_ROOT}/Scratch")
file(GLOB _sparkle_legacy_build_roots LIST_DIRECTORIES TRUE "${SPARKLE_REPOSITORY_ROOT}/build-*")
file(GLOB _sparkle_legacy_root_build_files LIST_DIRECTORIES FALSE
    "${SPARKLE_REPOSITORY_ROOT}/*.pdb"
    "${SPARKLE_REPOSITORY_ROOT}/*.sln"
    "${SPARKLE_REPOSITORY_ROOT}/*.slnx"
    "${SPARKLE_REPOSITORY_ROOT}/*.vcxproj"
    "${SPARKLE_REPOSITORY_ROOT}/*.vcxproj.filters"
    "${SPARKLE_REPOSITORY_ROOT}/*.vcxproj.user"
    "${SPARKLE_REPOSITORY_ROOT}/cmake_install.cmake"
    "${SPARKLE_REPOSITORY_ROOT}/Makefile")
file(GLOB _sparkle_legacy_project_outputs LIST_DIRECTORIES TRUE
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/build"
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/Cooked"
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/cooked"
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/logs"
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/StreamlineLogs"
    "${SPARKLE_REPOSITORY_ROOT}/Projects/*/imgui.ini")
list(APPEND _sparkle_legacy_generated_paths
    ${_sparkle_legacy_build_roots}
    ${_sparkle_legacy_root_build_files}
    ${_sparkle_legacy_project_outputs})

set(_sparkle_existing_legacy_generated_paths)
foreach(_sparkle_legacy_path IN LISTS _sparkle_legacy_generated_paths)
    if(EXISTS "${_sparkle_legacy_path}")
        list(APPEND _sparkle_existing_legacy_generated_paths "${_sparkle_legacy_path}")
    endif()
endforeach()
if(_sparkle_existing_legacy_generated_paths)
    list(JOIN _sparkle_existing_legacy_generated_paths "\n  - " _sparkle_legacy_path_list)
    message(FATAL_ERROR
        "Legacy generated paths exist outside Sparkle's owned output roots:\n  - ${_sparkle_legacy_path_list}\n"
        "Use SparkleLauncher cleanup after preserving any wanted captures. New builds must write only to build/, artifacts/, "
        "or the documented per-user state root.")
endif()

# A launcher built before the per-user logging cutover creates repository-root
# logs while bootstrapping its replacement. Keep this path visible and warn, but
# do not strand that one-way self-update route with a configure failure.
if(EXISTS "${SPARKLE_REPOSITORY_ROOT}/logs")
    message(WARNING
        "Legacy repository logs exist at '${SPARKLE_REPOSITORY_ROOT}/logs'. The current launcher can finish its self-update, then "
        "the Launcher Logs cleanup removes this transition output. Newly built processes write logs beneath the per-user state root.")
endif()

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

message(STATUS
    "Sparkle roots: build=${_sparkle_configured_build_root}; artifacts=${_sparkle_active_artifact_root}; "
    "dev=${_sparkle_product_root}")
message(STATUS "Sparkle artifact variant: ${SPARKLE_ARTIFACT_VARIANT}")
