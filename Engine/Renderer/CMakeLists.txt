# SparkleRenderer Module
# High-level rendering: scene/view preparation, frame-graph assembly, passes, materials, and textures
# Dependencies: SparkleCore, SparklePlatform, SparkleRHI, SparkleGameFramework runtime scene types
# Renderer consumes runtime scene/material data only. Source import, conversion,
# and authoring-side dependencies must stay out of this module.
# Renderer-owned shader payload layouts live under Private/ShaderData; RHI owns only binding/resource contracts.

file(GLOB_RECURSE SPARKLE_RENDERER_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_RENDERER_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_RENDERER_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ShaderRegistrations.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/NvidiaStreamlineProviders.cmake")

set(_sparkle_capture_controls ${SPARKLE_RENDERER_PRIVATE_SOURCES})
list(FILTER _sparkle_capture_controls INCLUDE REGEX "/Private/Diagnostics/ExternalCapture/")
list(FILTER SPARKLE_RENDERER_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Diagnostics/ExternalCapture/")
sparkle_add_engine_module(SparkleRenderer SPARKLE_RENDERER_EXPORTS
        ${SPARKLE_RENDERER_PUBLIC_HEADERS}
        ${SPARKLE_RENDERER_PRIVATE_SOURCES}
        ${SPARKLE_RENDERER_PRIVATE_HEADERS}
    )

target_include_directories(SparkleRenderer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        # Engine root for module-prefixed includes
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
)

target_compile_features(SparkleRenderer PUBLIC cxx_std_20)

# Precompiled header (per-module PCH)
sparkle_target_precompile_headers(SparkleRenderer
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

# Renderer publicly exposes Core event handles and the RHI-facing render seam.
# Platform and runtime scene modules remain implementation-only.
target_link_libraries(SparkleRenderer
    PUBLIC
        SparkleCore
        SparkleRHI
    PRIVATE
        SparklePlatform
        SparkleGameFramework
        enkiTS
        SparkleRendererNvidiaStreamlineProviders
        imgui
)

if(SPARKLE_BUILD_SHARED)
    target_link_libraries(SparkleRenderer PRIVATE SparkleRendererShaderRegistrations)
else()
    target_link_libraries(SparkleRenderer PUBLIC SparkleRendererShaderRegistrationObjects)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_RENDERER_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_RENDERER_PRIVATE_SOURCES} ${SPARKLE_RENDERER_PRIVATE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/Upscaling/NvidiaDlss PREFIX "Private\\Upscaling\\NvidiaDlss" FILES ${SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_SOURCES} ${SPARKLE_RENDERER_UPSCALING_NVIDIA_DLSS_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/RayReconstruction/NvidiaDlssRayReconstruction PREFIX "Private\\RayReconstruction\\NvidiaDlssRayReconstruction" FILES ${SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_SOURCES} ${SPARKLE_RENDERER_NVIDIA_RAY_RECONSTRUCTION_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/Streamline PREFIX "Private\\Streamline" FILES ${SPARKLE_RENDERER_STREAMLINE_SHARED_SOURCES} ${SPARKLE_RENDERER_STREAMLINE_SHARED_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_RENDERER_SHADER_REGISTRATION_SOURCES})

target_compile_definitions(SparkleRenderer PRIVATE SPARKLE_WITH_EXTERNAL_CAPTURE=$<AND:$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>,$<CONFIG:DebugEditor,DevelopmentEditor>>)

foreach(_capture_source IN LISTS _sparkle_capture_controls)
 target_sources(SparkleRenderer PRIVATE "$<$<AND:$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>,$<CONFIG:DebugEditor,DevelopmentEditor>>:${_capture_source}>")
endforeach()
