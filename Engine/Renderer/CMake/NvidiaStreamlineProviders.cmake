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
sparkle_enable_parallel_compilation(SparkleRendererNvidiaStreamlineProviders)

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

    if(NOT SPARKLE_ENABLE_NVIDIA_STREAMLINE)
        return()
    endif()

    if(NOT TARGET NVIDIA::Streamline)
        message(FATAL_ERROR "NVIDIA Streamline SDK target is unavailable for ${product_target}")
    endif()
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

    set(_notice_directory "$<TARGET_FILE_DIR:${product_target}>/Notices/NVIDIA")
    add_custom_target(${product_target}NvidiaNotices
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_notice_directory}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_notice_sources} "${_notice_directory}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/NVIDIA/README.md"
            "${_notice_directory}/README.md"
        COMMENT "Staging NVIDIA runtime notices for ${product_target}"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_dependencies(${product_target} ${product_target}NvidiaNotices)

    foreach(runtime_dll IN LISTS _runtime_files)
        if(NOT EXISTS "${runtime_dll}")
            message(FATAL_ERROR "NVIDIA Streamline runtime DLL is missing: '${runtime_dll}'")
        endif()
    endforeach()
    if(_runtime_files)
        add_custom_command(TARGET ${product_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${_runtime_files} "$<TARGET_FILE_DIR:${product_target}>"
            COMMENT "Staging NVIDIA Streamline runtime for ${product_target}"
            COMMAND_EXPAND_LISTS
            VERBATIM)
    endif()
endfunction()
