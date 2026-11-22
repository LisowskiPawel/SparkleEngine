file(GLOB_RECURSE SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Upscaling/NvidiaDlss/*.cpp
)

file(GLOB_RECURSE SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Upscaling/NvidiaDlss/*.h
)

file(GLOB_RECURSE SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/RayReconstruction/NvidiaDlssRayReconstruction/*.cpp
)

file(GLOB_RECURSE SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/RayReconstruction/NvidiaDlssRayReconstruction/*.h
)

file(GLOB_RECURSE SPARKLE_RENDERER_STREAMLINE_SHARED_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Streamline/*.cpp
)

file(GLOB_RECURSE SPARKLE_RENDERER_STREAMLINE_SHARED_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Streamline/*.h
)

set(SPARKLE_RENDERER_NVIDIA_STREAMLINE_PROVIDER_SOURCES
    ${SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_SOURCES}
    ${SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_SOURCES}
    ${SPARKLE_RENDERER_STREAMLINE_SHARED_SOURCES}
)

set(SPARKLE_RENDERER_NVIDIA_STREAMLINE_PROVIDER_HEADERS
    ${SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_HEADERS}
    ${SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_HEADERS}
    ${SPARKLE_RENDERER_STREAMLINE_SHARED_HEADERS}
)

list(FILTER SPARKLE_RENDERER_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Upscaling/NvidiaDlss/")
list(FILTER SPARKLE_RENDERER_PRIVATE_SOURCES EXCLUDE REGEX "/Private/RayReconstruction/NvidiaDlssRayReconstruction/")
list(FILTER SPARKLE_RENDERER_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Streamline/")
list(FILTER SPARKLE_RENDERER_PRIVATE_HEADERS EXCLUDE REGEX "/Private/Upscaling/NvidiaDlss/")
list(FILTER SPARKLE_RENDERER_PRIVATE_HEADERS EXCLUDE REGEX "/Private/RayReconstruction/NvidiaDlssRayReconstruction/")
list(FILTER SPARKLE_RENDERER_PRIVATE_HEADERS EXCLUDE REGEX "/Private/Streamline/")

add_library(SparkleRendererNvidiaStreamlineProviders STATIC
    ${SPARKLE_RENDERER_NVIDIA_STREAMLINE_PROVIDER_SOURCES}
    ${SPARKLE_RENDERER_NVIDIA_STREAMLINE_PROVIDER_HEADERS}
)

target_include_directories(SparkleRendererNvidiaStreamlineProviders
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
)

target_compile_features(SparkleRendererNvidiaStreamlineProviders PUBLIC cxx_std_20)
sparkle_configure_native_target(SparkleRendererNvidiaStreamlineProviders)

target_link_libraries(SparkleRendererNvidiaStreamlineProviders
    PUBLIC
        SparkleCore
        SparkleRHI
    PRIVATE
        SparklePlatform
)

if(SPARKLE_ENABLE_NVIDIA_STREAMLINE AND TARGET NVIDIA::Streamline)
    target_link_libraries(SparkleRendererNvidiaStreamlineProviders PRIVATE NVIDIA::Streamline)
    target_compile_definitions(SparkleRendererNvidiaStreamlineProviders PRIVATE SPARKLE_WITH_NVIDIA_STREAMLINE=1)
else()
    target_compile_definitions(SparkleRendererNvidiaStreamlineProviders PRIVATE SPARKLE_WITH_NVIDIA_STREAMLINE=0)
endif()

sparkle_target_precompile_headers(SparkleRendererNvidiaStreamlineProviders
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

function(sparkle_stage_nvidia_streamline_runtime product_target)
    if(NOT TARGET ${product_target})
        message(FATAL_ERROR "Unknown Sparkle product target '${product_target}'")
    endif()

    # Read the same declared payload when disabled, so old owned files can retire.
    set(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY TRUE)
    include("${SPARKLE_REPOSITORY_ROOT}/ThirdParty/nvidia-streamline/Dependency.cmake")
    set(_stale_files)
    foreach(_runtime IN LISTS _sparkle_streamline_runtime_files)
        get_filename_component(_name "${_runtime}" NAME)
        list(APPEND _stale_files "${_name}")
    endforeach()
    foreach(_notice IN LISTS _sparkle_streamline_notice_files)
        get_filename_component(_name "${_notice}" NAME)
        list(APPEND _stale_files "Notices/NVIDIA/${_name}")
    endforeach()
    list(APPEND _stale_files "Notices/NVIDIA/README.md")
    if(NOT SPARKLE_ENABLE_NVIDIA_STREAMLINE)
        sparkle_stage_runtime_files(${product_target} Nvidia STALE_FILES ${_stale_files})
        return()
    endif()

    if(NOT TARGET NVIDIA::Streamline)
        message(FATAL_ERROR "NVIDIA Streamline SDK target is unavailable for ${product_target}")
    endif()
    sparkle_target_source_package_dependencies(${product_target} NVIDIA::Streamline)
    get_target_property(_runtime_files NVIDIA::Streamline SPARKLE_STREAMLINE_RUNTIME_FILES)
    get_target_property(_notice_sources NVIDIA::Streamline SPARKLE_STREAMLINE_NOTICE_FILES)
    if(_runtime_files STREQUAL "_runtime_files-NOTFOUND" OR NOT _notice_sources)
        message(FATAL_ERROR "NVIDIA Streamline SDK deployment metadata is unavailable for ${product_target}")
    endif()

    foreach(_notice_source IN LISTS _notice_sources)
        if(NOT EXISTS "${_notice_source}")
            message(FATAL_ERROR "NVIDIA Streamline runtime notice is missing: '${_notice_source}'")
        endif()
    endforeach()

    foreach(runtime_dll IN LISTS _runtime_files)
        if(NOT EXISTS "${runtime_dll}")
            message(FATAL_ERROR "NVIDIA Streamline runtime DLL is missing: '${runtime_dll}'")
        endif()
    endforeach()
    set(_staging_files ${_runtime_files})
    foreach(_notice IN LISTS _notice_sources)
        get_filename_component(_name "${_notice}" NAME)
        list(APPEND _staging_files "${_notice}|Notices/NVIDIA/${_name}")
    endforeach()
    list(APPEND _staging_files "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/NVIDIA/README.md|Notices/NVIDIA/README.md")
    sparkle_stage_runtime_files(${product_target} Nvidia FILES ${_staging_files} STALE_FILES ${_stale_files})
    set_property(TARGET ${product_target} APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES
        "$<PATH:GET_FILENAME,${_runtime_files}>")
endfunction()
