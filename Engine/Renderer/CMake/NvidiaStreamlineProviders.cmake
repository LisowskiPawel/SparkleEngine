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

