set(SPARKLE_NVIDIA_NVAPI_GIT_TAG "9b181ea572f680327fe01a14a0f1f41c78034104" CACHE STRING "Official NVIDIA NVAPI source revision")

function(sparkle_populate_nvidia_nvapi)
    set(_nvapi_source_dir "${FETCHCONTENT_BASE_DIR}/sparkle_nvapi-src")
    sparkle_prepare_git_fetchcontent_source(
        "sparkle_nvapi"
        DISPLAY_NAME "NVIDIA NVAPI"
        SOURCE_DIR "${_nvapi_source_dir}"
        EXPECTED_REVISION "${SPARKLE_NVIDIA_NVAPI_GIT_TAG}"
        REQUIRED_PATHS
        "nvapi.h"
    "amd64/nvapi64.lib")

    FetchContent_Declare(sparkle_nvapi
        GIT_REPOSITORY https://github.com/NVIDIA/nvapi.git
        GIT_TAG        ${SPARKLE_NVIDIA_NVAPI_GIT_TAG}
        GIT_SHALLOW    FALSE
        GIT_PROGRESS   ${_sparkle_git_progress}
    )
    FetchContent_Populate(sparkle_nvapi)
    set(sparkle_nvapi_SOURCE_DIR "${sparkle_nvapi_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

if(SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "nvidia-nvapi")
    sparkle_log_dependency_step("NVIDIA NVAPI SDK" "git 9b181ea" "source SDK" "D3D12 NVAPI headers and import library" "https://github.com/NVIDIA/nvapi.git")
    sparkle_populate_nvidia_nvapi()
    sparkle_log_dependency_ready("NVIDIA NVAPI SDK" "${sparkle_nvapi_SOURCE_DIR}" "source SDK")
    return()
endif()
