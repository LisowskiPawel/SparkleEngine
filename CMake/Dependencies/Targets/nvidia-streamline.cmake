sparkle_log_dependency_step("NVIDIA Streamline SDK" "v2.11.1" "~217 MB" "DLSS external provider SDK headers, import library, and runtime DLLs" "https://github.com/NVIDIA-RTX/Streamline/releases")

set(_sparkle_streamline_url "https://github.com/NVIDIA-RTX/Streamline/releases/download/v2.11.1/streamline-sdk-v2.11.1.zip")
set(_sparkle_streamline_zip "${FETCHCONTENT_BASE_DIR}/streamline-sdk-v2.11.1.zip")
set(_sparkle_streamline_root "${FETCHCONTENT_BASE_DIR}/streamline-sdk-src")
set(_sparkle_streamline_runtime_files
    "bin/x64/sl.interposer.dll"
    "bin/x64/sl.common.dll"
    "bin/x64/sl.dlss.dll"
    "bin/x64/sl.dlss_d.dll"
    "bin/x64/sl.pcl.dll"
    "bin/x64/sl.reflex.dll"
    "bin/x64/NvLowLatencyVk.dll"
    "bin/x64/nvngx_dlss.dll"
    "bin/x64/nvngx_dlssd.dll"
)
# Runtime plugins carry notices beyond the SDK's top-level MIT license.
set(_sparkle_streamline_notice_files
    "license.txt"
    "3rd-party-licenses.md"
    "bin/x64/nis.license.txt"
    "bin/x64/nvngx_dlss.license.txt"
    "bin/x64/reflex.license.txt"
)

sparkle_ensure_archive_dependency(
    DISPLAY_NAME "NVIDIA Streamline SDK"
    URL "${_sparkle_streamline_url}"
    ARCHIVE_PATH "${_sparkle_streamline_zip}"
    ROOT_PATH "${_sparkle_streamline_root}"
    EXPECTED_HASH "SHA256=0C1D562E59557434CABFB8997157CB8C04FC7D23F077C8BDF5260975B73DFB89"
    REQUIRED_PATHS "include/sl.h" "lib/x64/sl.interposer.lib"
        ${_sparkle_streamline_runtime_files} ${_sparkle_streamline_notice_files}
)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("NVIDIA Streamline SDK" "${_sparkle_streamline_root}" "~217 MB release SDK")
    return()
endif()

if(NOT TARGET NVIDIA::Streamline)
    add_library(NVIDIA::Streamline INTERFACE IMPORTED GLOBAL)
    set_target_properties(NVIDIA::Streamline PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_sparkle_streamline_root}/include"
        INTERFACE_LINK_LIBRARIES "${_sparkle_streamline_root}/lib/x64/sl.interposer.lib"
    )
endif()

list(TRANSFORM _sparkle_streamline_runtime_files PREPEND "${_sparkle_streamline_root}/")
list(TRANSFORM _sparkle_streamline_notice_files PREPEND "${_sparkle_streamline_root}/")
set_target_properties(NVIDIA::Streamline PROPERTIES
    SPARKLE_STREAMLINE_RUNTIME_FILES "${_sparkle_streamline_runtime_files}"
    SPARKLE_STREAMLINE_NOTICE_FILES "${_sparkle_streamline_notice_files}")

sparkle_log_dependency_ready("NVIDIA Streamline SDK" "${_sparkle_streamline_root}" "~217 MB release SDK")
