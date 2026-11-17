sparkle_log_dependency_step("NVIDIA Streamline SDK" "v2.11.1" "~217 MB" "DLSS external provider SDK headers, import library, and runtime DLLs" "https://github.com/NVIDIA-RTX/Streamline/releases")

set(_sparkle_streamline_url "https://github.com/NVIDIA-RTX/Streamline/releases/download/v2.11.1/streamline-sdk-v2.11.1.zip")
set(_sparkle_streamline_zip "${FETCHCONTENT_BASE_DIR}/streamline-sdk-v2.11.1.zip")
set(_sparkle_streamline_root "${FETCHCONTENT_BASE_DIR}/streamline-sdk-src")
set(_sparkle_streamline_required_paths
    "include/sl.h"
    "lib/x64/sl.interposer.lib"
    "bin/x64/sl.interposer.dll"
    "bin/x64/sl.common.dll"
    "bin/x64/sl.dlss.dll"
    "bin/x64/sl.dlss_d.dll"
    "bin/x64/sl.pcl.dll"
    "bin/x64/sl.reflex.dll"
    "bin/x64/NvLowLatencyVk.dll"
    "bin/x64/nvngx_dlss.dll"
    "bin/x64/nvngx_dlssd.dll"
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
    REQUIRED_PATHS ${_sparkle_streamline_required_paths}
)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("NVIDIA Streamline SDK" "${_sparkle_streamline_root}" "~217 MB release SDK")
    return()
endif()

set(SPARKLE_NVIDIA_STREAMLINE_ROOT "${_sparkle_streamline_root}" CACHE PATH "NVIDIA Streamline SDK root used by Sparkle DLSS integration." FORCE)
set(SPARKLE_NVIDIA_STREAMLINE_BIN_DIR "${_sparkle_streamline_root}/bin/x64" CACHE PATH "NVIDIA Streamline runtime DLL directory." FORCE)
set(SPARKLE_NVIDIA_STREAMLINE_INCLUDE_DIR "${_sparkle_streamline_root}/include" CACHE PATH "NVIDIA Streamline include directory." FORCE)
set(SPARKLE_NVIDIA_STREAMLINE_INTERPOSER_LIB "${_sparkle_streamline_root}/lib/x64/sl.interposer.lib" CACHE FILEPATH "NVIDIA Streamline interposer import library." FORCE)

if(NOT TARGET NVIDIA::Streamline)
    add_library(NVIDIA::Streamline INTERFACE IMPORTED GLOBAL)
    set_target_properties(NVIDIA::Streamline PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SPARKLE_NVIDIA_STREAMLINE_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${SPARKLE_NVIDIA_STREAMLINE_INTERPOSER_LIB}"
    )
endif()

set(SPARKLE_NVIDIA_STREAMLINE_RUNTIME_DLLS
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.interposer.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.common.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.dlss.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.dlss_d.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.pcl.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/sl.reflex.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/NvLowLatencyVk.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/nvngx_dlss.dll"
    "${SPARKLE_NVIDIA_STREAMLINE_BIN_DIR}/nvngx_dlssd.dll"
    CACHE STRING "NVIDIA Streamline DLSS SR and Ray Reconstruction runtime DLLs staged beside Sparkle products."
    FORCE
)

sparkle_log_dependency_ready("NVIDIA Streamline SDK" "${SPARKLE_NVIDIA_STREAMLINE_ROOT}" "~217 MB release SDK")
