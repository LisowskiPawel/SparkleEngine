# RHI selects the backend/SDK root; this recipe owns its source and imported target.
function(sparkle_populate_nvidia_nvapi)
    sparkle_declare_git_dependency(sparkle_nvapi
        https://github.com/NVIDIA/nvapi.git 9b181ea572f680327fe01a14a0f1f41c78034104
        ID nvidia-nvapi VERSION 9b181ea572f680327fe01a14a0f1f41c78034104
        PACKAGE_NAME "NVIDIA NVAPI" LICENSE_ID MIT
        REQUIRED_PATHS nvapi.h nvapi_interface.h amd64/nvapi64.lib License.txt)
    set(sparkle_nvapi_SOURCE_DIR "${sparkle_nvapi_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "nvidia-nvapi")
    sparkle_populate_nvidia_nvapi()
    return()
endif()

function(sparkle_configure_nvidia_nvapi_target)
    if(SPARKLE_RHI_D3D12_NVAPI_FETCH_FROM_GITHUB)
        sparkle_populate_nvidia_nvapi()
        set(SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR "${sparkle_nvapi_SOURCE_DIR}" CACHE PATH "NVAPI headers for the D3D12 backend" FORCE)
        set(SPARKLE_RHI_D3D12_NVAPI_LIBRARY "${sparkle_nvapi_SOURCE_DIR}/amd64/nvapi64.lib" CACHE FILEPATH "NVAPI library for the D3D12 backend" FORCE)
    endif()
    set(sdk_inputs "${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}/nvapi.h"
        "${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}/nvapi_interface.h"
        "${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}/License.txt" "${SPARKLE_RHI_D3D12_NVAPI_LIBRARY}")
    foreach(path IN LISTS sdk_inputs)
        if(NOT EXISTS "${path}" OR IS_DIRECTORY "${path}")
            message(FATAL_ERROR "The selected NVAPI SDK is incomplete: '${path}'.")
        endif()
        file(SIZE "${path}" size)
        if(size EQUAL 0)
            message(FATAL_ERROR "The selected NVAPI SDK has an empty input: '${path}'.")
        endif()
    endforeach()
    add_library(SparkleNVAPI STATIC IMPORTED GLOBAL)
    set_target_properties(SparkleNVAPI PROPERTIES
        IMPORTED_LOCATION "${SPARKLE_RHI_D3D12_NVAPI_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}")
    if(SPARKLE_RHI_D3D12_NVAPI_FETCH_FROM_GITHUB)
        sparkle_add_dependency_source_package(SparkleNVAPI nvidia-nvapi)
    else()
        set(sdk_identity)
        foreach(path IN LISTS sdk_inputs)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${path}")
            file(SHA256 "${path}" input_hash)
            string(APPEND sdk_identity "${input_hash}\n")
        endforeach()
        string(SHA256 sdk_hash "${sdk_identity}")
        sparkle_add_source_package(SparkleNVAPI "NVIDIA NVAPI" "sha256:${sdk_hash}"
            MIT "https://github.com/NVIDIA/nvapi")
    endif()
endfunction()
