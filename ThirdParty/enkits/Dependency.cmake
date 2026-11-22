set(SPARKLE_ENKITS_REVISION "0289cf6ffce86697e1f46cb4eeca1c946bffec71")
set(SPARKLE_ENKITS_PATCH "ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch")
set(SPARKLE_ENKITS_PATCH_SHA256 "8d066c5a4227d6943a7f3cd68176e5f5c2791e257875ef194e0e9ab43d46b201")
set(SPARKLE_ENKITS_VERSION "1.12+sparkle.runtime.${SPARKLE_ENKITS_PATCH_SHA256}")
set(SPARKLE_ENKITS_REQUIRED_PATHS
    CMakeLists.txt License.txt README.md src/TaskScheduler.h src/LockLessMultiReadPipe.h
    src/TaskScheduler.cpp src/TaskScheduler_c.h src/TaskScheduler_c.cpp)

set(_sparkle_enkits_patch "${SPARKLE_REPOSITORY_ROOT}/${SPARKLE_ENKITS_PATCH}")
if(NOT EXISTS "${_sparkle_enkits_patch}")
    message(FATAL_ERROR "The qualified enkiTS runtime patch is missing: '${_sparkle_enkits_patch}'.")
endif()
file(SHA256 "${_sparkle_enkits_patch}" _sparkle_enkits_patch_sha256)
if(NOT _sparkle_enkits_patch_sha256 STREQUAL SPARKLE_ENKITS_PATCH_SHA256)
    message(FATAL_ERROR "enkiTS runtime patch differs from the qualified identity. Patch and cache preserved.")
endif()

sparkle_declare_git_dependency(enkits https://github.com/dougbinks/enkiTS.git
    ${SPARKLE_ENKITS_REVISION}
    ID enkits VERSION "${SPARKLE_ENKITS_VERSION}" PACKAGE_NAME "enkiTS (Sparkle runtime fork)" LICENSE_ID "Zlib"
    REQUIRED_PATHS ${SPARKLE_ENKITS_REQUIRED_PATHS} GIT_CONFIG core.autocrlf=false)
if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/RuntimeSource.cmake")

function(sparkle_add_enkits)
    set(_source "${enkits_BINARY_DIR}/source")
    sparkle_prepare_enkits_source("${_source}")

    set(ENKITS_BUILD_C_INTERFACE ON)
    set(ENKITS_BUILD_EXAMPLES OFF)
    set(ENKITS_BUILD_SHARED ${SPARKLE_BUILD_SHARED})
    set(ENKITS_INSTALL OFF)
    set(ENKITS_SANITIZE OFF)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    sparkle_add_dependency_subdirectory("${_source}" "${enkits_BINARY_DIR}/native" EXCLUDE_FROM_ALL)
    if(MSVC)
        set_property(SOURCE "${_source}/src/TaskScheduler_c.cpp"
            TARGET_DIRECTORY enkiTS APPEND PROPERTY COMPILE_OPTIONS /EHc-)
    endif()
    set_target_properties(enkiTS PROPERTIES FOLDER "ThirdParty/enkits"
        MSVC_RUNTIME_LIBRARY "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    sparkle_configure_runtime_support_artifacts(enkiTS)
    sparkle_add_dependency_source_package(enkiTS enkits)
endfunction()
sparkle_add_enkits()
