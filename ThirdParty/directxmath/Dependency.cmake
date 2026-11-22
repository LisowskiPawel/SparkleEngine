if(TARGET SparkleDirectXMath)
    return()
endif()
set(_windows_sdk_version "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
if(NOT _windows_sdk_version)
    string(REGEX REPLACE "[/\\]+$" "" _windows_sdk_version "$ENV{WindowsSDKVersion}")
endif()
unset(SPARKLE_DIRECTXMATH_INCLUDE_DIR)
unset(SPARKLE_DIRECTXMATH_INCLUDE_DIR CACHE)
find_path(SPARKLE_DIRECTXMATH_INCLUDE_DIR DirectXMath.h
    HINTS "$ENV{WindowsSdkDir}/Include/${_windows_sdk_version}/um"
        "C:/Program Files (x86)/Windows Kits/10/Include/${_windows_sdk_version}/um"
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES} ENV INCLUDE
    REQUIRED NO_DEFAULT_PATH)
file(GLOB _math_inputs CONFIGURE_DEPENDS "${SPARKLE_DIRECTXMATH_INCLUDE_DIR}/DirectXMath*.inl"
    "${SPARKLE_DIRECTXMATH_INCLUDE_DIR}/DirectXMath.h"
    "${SPARKLE_DIRECTXMATH_INCLUDE_DIR}/DirectXPackedVector.h"
    "${SPARKLE_DIRECTXMATH_INCLUDE_DIR}/DirectXPackedVector.inl")
set(_math_identity)
foreach(input IN LISTS _math_inputs)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${input}")
    file(SHA256 "${input}" hash)
    string(APPEND _math_identity "${hash}\n")
endforeach()
string(SHA256 _math_identity "${_math_identity}")
add_library(SparkleDirectXMath INTERFACE)
target_include_directories(SparkleDirectXMath SYSTEM INTERFACE "${SPARKLE_DIRECTXMATH_INCLUDE_DIR}")
sparkle_add_source_package(SparkleDirectXMath DirectXMath "sha256:${_math_identity}"
    MIT "https://github.com/microsoft/DirectXMath")
set_target_properties(SparkleDirectXMath PROPERTIES FOLDER "ThirdParty/directxmath")
