# Select source inputs; each recipe owns its pin, required files and native targets.
set(_sparkle_source_dependencies
    cli11
    nlohmann_json
    nvidia-nvapi
    imgui
    cgltf
    mikktspace
    stb
    tinyexr
    spdlog
    zlib
    assimp
    directxtex
    ktx
    spirv-reflect
    editor-icons
    nvidia-streamline
    winpixeventruntime
)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY IN_LIST _sparkle_source_dependencies)
        message(FATAL_ERROR "Unknown source dependency '${SPARKLE_SYNC_SOURCE_DEPENDENCY}'.")
    endif()
    set(_sparkle_source_dependencies "${SPARKLE_SYNC_SOURCE_DEPENDENCY}")
else()
    if(DEFINED SPARKLE_SOURCE_DEPENDENCY_ALLOWLIST)
        foreach(_dependency IN LISTS SPARKLE_SOURCE_DEPENDENCY_ALLOWLIST)
            if(NOT _dependency IN_LIST _sparkle_source_dependencies)
                message(FATAL_ERROR "Unknown source dependency '${_dependency}' in SPARKLE_SOURCE_DEPENDENCY_ALLOWLIST.")
            endif()
        endforeach()
        foreach(_dependency IN LISTS _sparkle_source_dependencies)
            if(NOT _dependency IN_LIST SPARKLE_SOURCE_DEPENDENCY_ALLOWLIST)
                list(REMOVE_ITEM _sparkle_source_dependencies "${_dependency}")
            endif()
        endforeach()
    endif()
    if(NOT SPARKLE_ENABLE_CONTENT_PIPELINE)
        list(REMOVE_ITEM _sparkle_source_dependencies cgltf mikktspace stb assimp directxtex ktx)
    endif()
    if(NOT SPARKLE_ENABLE_SHADER_COMPILER)
        list(REMOVE_ITEM _sparkle_source_dependencies spirv-reflect)
    endif()
    if(NOT SPARKLE_ENABLE_NVIDIA_STREAMLINE)
        list(REMOVE_ITEM _sparkle_source_dependencies nvidia-streamline)
    endif()
    if(NOT SPARKLE_RHI_WITH_D3D12_PIX_EVENTS AND NOT SPARKLE_RHI_WITH_EXTERNAL_CAPTURE)
        list(REMOVE_ITEM _sparkle_source_dependencies winpixeventruntime)
    endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/DependencySetup.cmake" NO_POLICY_SCOPE)
include("${CMAKE_CURRENT_LIST_DIR}/DependencyCache.cmake")

if(SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    message(STATUS "Third-party dependencies: checking cache (set SPARKLE_VERBOSE_DEPENDENCIES=ON for details)")
else()
    message(STATUS "Source dependency sync: checking ${SPARKLE_SYNC_SOURCE_DEPENDENCY}")
endif()

# Source-only acquisition skips LFS media and tolerates machines without git-lfs.
# Restore the caller's environment after every recipe, including selective sync.
foreach(_variable IN ITEMS GIT_LFS_SKIP_SMUDGE GIT_CONFIG_PARAMETERS)
    set(_sparkle_had_${_variable} FALSE)
    if(DEFINED ENV{${_variable}})
        set(_sparkle_had_${_variable} TRUE)
        set(_sparkle_saved_${_variable} "$ENV{${_variable}}")
    endif()
endforeach()
set(ENV{GIT_LFS_SKIP_SMUDGE} 1)
set(ENV{GIT_CONFIG_PARAMETERS} "'filter.lfs.process=' 'filter.lfs.smudge=' 'filter.lfs.clean=' 'filter.lfs.required=false' 'core.longpaths=true'")

foreach(_dependency IN LISTS _sparkle_source_dependencies)
    include("${CMAKE_CURRENT_LIST_DIR}/Targets/${_dependency}.cmake")
endforeach()

foreach(_variable IN ITEMS GIT_LFS_SKIP_SMUDGE GIT_CONFIG_PARAMETERS)
    if(_sparkle_had_${_variable})
        set(ENV{${_variable}} "${_sparkle_saved_${_variable}}")
    else()
        unset(ENV{${_variable}})
    endif()
endforeach()
message(STATUS "Third-party dependencies: ready")
