# The shared source cache has one pinned identity, matching Launcher acquisition metadata.
set(_sparkle_nvapi_revision "9b181ea572f680327fe01a14a0f1f41c78034104")

function(sparkle_populate_nvidia_nvapi)
    set(_nvapi_source_dir "${FETCHCONTENT_BASE_DIR}/sparkle_nvapi-src")
    sparkle_prepare_git_fetchcontent_source(
        "sparkle_nvapi"
        DISPLAY_NAME "NVIDIA NVAPI"
        SOURCE_DIR "${_nvapi_source_dir}"
        EXPECTED_REVISION "${_sparkle_nvapi_revision}"
        REQUIRED_PATHS
        "nvapi.h"
    "amd64/nvapi64.lib")

    FetchContent_Declare(sparkle_nvapi
        GIT_REPOSITORY https://github.com/NVIDIA/nvapi.git
        GIT_TAG        ${_sparkle_nvapi_revision}
        GIT_SHALLOW    FALSE
        GIT_PROGRESS   FALSE
    )
    FetchContent_Populate(sparkle_nvapi)
    set(sparkle_nvapi_SOURCE_DIR "${sparkle_nvapi_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

if(SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "nvidia-nvapi")
    sparkle_populate_nvidia_nvapi()
    return()
endif()
