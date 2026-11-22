set(_sparkle_pix_root "${FETCHCONTENT_BASE_DIR}/winpixeventruntime-src")
set(_sparkle_pix_version "1.0.240308001")
set(_sparkle_pix_archive "${FETCHCONTENT_BASE_DIR}/winpixeventruntime.${_sparkle_pix_version}.nupkg")
set(_sparkle_pix_required_paths Include/WinPixEventRuntime/pix3.h bin/x64/WinPixEventRuntime.lib bin/x64/WinPixEventRuntime.dll license.txt)
get_filename_component(_cache_directory "${_sparkle_pix_root}" NAME)
get_filename_component(_archive_name "${_sparkle_pix_archive}" NAME)
sparkle_record_source_dependency(winpixeventruntime VERSION "${_sparkle_pix_version}"
    PACKAGE_NAME "WinPixEventRuntime" LICENSE_ID "MIT" LOCATION "https://www.nuget.org/packages/WinPixEventRuntime/${_sparkle_pix_version}"
    CACHE_DIRECTORY "${_cache_directory}" REQUIRED_PATHS ${_sparkle_pix_required_paths} CACHE_FILES "${_archive_name}")
if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY)
    return()
endif()
sparkle_ensure_archive_dependency(
    DISPLAY_NAME "WinPixEventRuntime"
    URL "https://api.nuget.org/v3-flatcontainer/winpixeventruntime/${_sparkle_pix_version}/winpixeventruntime.${_sparkle_pix_version}.nupkg"
    ARCHIVE_PATH "${_sparkle_pix_archive}"
    ROOT_PATH "${_sparkle_pix_root}"
    EXPECTED_HASH "SHA256=726acc93d6968e2146261a1e415521747d50ad69894c2b42b5d0d4c29fd66ec4"
    REQUIRED_PATHS ${_sparkle_pix_required_paths}
)
if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()
if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_SYSTEM_PROCESSOR MATCHES "[Aa][Rr][Mm]" OR CMAKE_GENERATOR_PLATFORM MATCHES "[Aa][Rr][Mm]")
    message(FATAL_ERROR "The selected WinPixEventRuntime integration requires Windows x64.")
endif()
add_library(Microsoft::WinPixEventRuntime SHARED IMPORTED GLOBAL)
set_target_properties(Microsoft::WinPixEventRuntime PROPERTIES
    IMPORTED_IMPLIB "${_sparkle_pix_root}/bin/x64/WinPixEventRuntime.lib"
    IMPORTED_LOCATION "${_sparkle_pix_root}/bin/x64/WinPixEventRuntime.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${_sparkle_pix_root}/Include/WinPixEventRuntime"
    SPARKLE_LICENSE_FILE "${_sparkle_pix_root}/license.txt"
)
sparkle_add_dependency_source_package(Microsoft::WinPixEventRuntime winpixeventruntime)
