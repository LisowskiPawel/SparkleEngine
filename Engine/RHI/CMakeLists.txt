# SparkleRHI Module
# Graphics API abstraction: common RHI contracts plus backend-private implementations.
# Dependencies: SparkleCore, SparklePlatform
# Renderer shader payload layouts stay in Renderer; RHI owns device/resource/pipeline contracts only.

include(FetchContent)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Contracts.cmake)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/BackendOptions.cmake")
include("${SPARKLE_REPOSITORY_ROOT}/ThirdParty/imgui/Backends.cmake")

file(GLOB_RECURSE SPARKLE_RHI_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_RHI_COMMON_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_RHI_COMMON_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

list(FILTER SPARKLE_RHI_COMMON_PRIVATE_SOURCES EXCLUDE REGEX "/Private/D3D12/")
list(FILTER SPARKLE_RHI_COMMON_PRIVATE_HEADERS EXCLUDE REGEX "/Private/D3D12/")
list(FILTER SPARKLE_RHI_COMMON_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Vulkan/")
list(FILTER SPARKLE_RHI_COMMON_PRIVATE_HEADERS EXCLUDE REGEX "/Private/Vulkan/")
list(FILTER SPARKLE_RHI_COMMON_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Diagnostics/")
list(FILTER SPARKLE_RHI_COMMON_PRIVATE_HEADERS EXCLUDE REGEX "/Private/Diagnostics/")

file(GLOB_RECURSE SPARKLE_RHI_DIAGNOSTICS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics/*.cpp
)

file(GLOB_RECURSE SPARKLE_RHI_DIAGNOSTICS_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics/*.h
)

if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    list(FILTER SPARKLE_RHI_DIAGNOSTICS_SOURCES EXCLUDE REGEX "/Diagnostics/GpuCrash/")
    list(FILTER SPARKLE_RHI_DIAGNOSTICS_HEADERS EXCLUDE REGEX "/Diagnostics/GpuCrash/")
endif()

add_library(SparkleRHICommon OBJECT
    ${SPARKLE_RHI_PUBLIC_HEADERS}
    ${SPARKLE_RHI_COMMON_PRIVATE_SOURCES}
    ${SPARKLE_RHI_COMMON_PRIVATE_HEADERS}
)

function(sparkle_configure_rhi_implementation_target target_name)
    sparkle_configure_native_target(${target_name})

    target_include_directories(${target_name}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
            # Engine root for module-prefixed includes
            ${CMAKE_CURRENT_SOURCE_DIR}/..
            ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
            ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Time
    )

    target_compile_features(${target_name} PUBLIC cxx_std_20)

    target_compile_definitions(${target_name}
        PRIVATE
            SPARKLE_RHI_WITH_D3D12=$<BOOL:${SPARKLE_RHI_WITH_D3D12}>
            SPARKLE_WITH_GPU_CRASH_DEBUGGING=$<BOOL:${SPARKLE_WITH_GPU_CRASH_DEBUGGING}>
            SPARKLE_WITH_EXTERNAL_CAPTURE=$<AND:$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>,$<CONFIG:DebugEditor,DevelopmentEditor>>
            SPARKLE_RHI_WITH_VULKAN=$<BOOL:${SPARKLE_RHI_WITH_VULKAN}>
            SPARKLE_RHI_DEFAULT_BACKEND_${SPARKLE_RHI_DEFAULT_BACKEND_UPPER}=1
            ENGINE_GPU_VALIDATION=$<NOT:$<CONFIG:ShippingEditor,ShippingGame>>
            ENGINE_REPORT_LIVE_OBJECTS=$<NOT:$<CONFIG:ShippingEditor,ShippingGame>>
    )

    if(SPARKLE_BUILD_SHARED)
        target_compile_definitions(${target_name} PRIVATE SPARKLE_RHI_EXPORTS)
    else()
        target_compile_definitions(${target_name} PRIVATE SPARKLE_STATIC)
    endif()

    if(SPARKLE_WITH_GPU_CRASH_DEBUGGING)
        target_include_directories(${target_name} PRIVATE "${SPARKLE_AFTERMATH_SDK_ROOT}/include")
        target_compile_definitions(${target_name} PRIVATE "SPARKLE_AFTERMATH_RUNTIME_SHA256=\"${SPARKLE_AFTERMATH_RUNTIME_SHA256}\"")
    endif()

endfunction()

function(sparkle_assert_rhi_target_excludes_backend_sources target_name backend_directory)
    get_target_property(_sparkle_target_sources ${target_name} SOURCES)
    if(NOT _sparkle_target_sources)
        return()
    endif()

    foreach(_sparkle_source IN LISTS _sparkle_target_sources)
        file(TO_CMAKE_PATH "${_sparkle_source}" _sparkle_normalized_source)
        if(_sparkle_normalized_source MATCHES "/Private/${backend_directory}/")
            message(FATAL_ERROR
                "RHI target '${target_name}' must not compile ${backend_directory} backend source '${_sparkle_normalized_source}'.")
        endif()
    endforeach()
endfunction()

sparkle_configure_rhi_implementation_target(SparkleRHICommon)

target_link_libraries(SparkleRHICommon
    PUBLIC
        SparkleCore
    PRIVATE
        SparklePlatform
        imgui
)

sparkle_target_precompile_headers(SparkleRHICommon
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

set(_sparkle_native_capture_sources ${SPARKLE_RHI_DIAGNOSTICS_SOURCES})
list(FILTER _sparkle_native_capture_sources INCLUDE REGEX "/Private/Diagnostics/ExternalCapture/")
list(FILTER SPARKLE_RHI_DIAGNOSTICS_SOURCES EXCLUDE REGEX "/Private/Diagnostics/ExternalCapture/")
add_library(SparkleRHIDiagnostics STATIC
    ${SPARKLE_RHI_DIAGNOSTICS_SOURCES}
    ${SPARKLE_RHI_DIAGNOSTICS_HEADERS}
)

sparkle_configure_rhi_implementation_target(SparkleRHIDiagnostics)

target_link_libraries(SparkleRHIDiagnostics
    PRIVATE
        SparkleCore
)

sparkle_target_precompile_headers(SparkleRHIDiagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

set(SPARKLE_RHI_BACKEND_TARGETS "")

if(SPARKLE_RHI_WITH_D3D12)
    include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/D3D12.cmake")
endif()
if(SPARKLE_RHI_WITH_VULKAN)
    include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/Vulkan.cmake")
endif()

if(SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    foreach(target_name IN ITEMS SparkleRHIDiagnostics ${SPARKLE_RHI_BACKEND_TARGETS})
        target_link_libraries(${target_name} PRIVATE nlohmann_json::nlohmann_json)
    endforeach()
endif()

sparkle_add_engine_module(SparkleRHI SPARKLE_RHI_EXPORTS
        ${SPARKLE_RHI_PUBLIC_HEADERS}
        $<TARGET_OBJECTS:SparkleRHICommon>
    )

target_include_directories(SparkleRHI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

target_compile_features(SparkleRHI PUBLIC cxx_std_20)
target_compile_definitions(SparkleRHI PUBLIC SPARKLE_WITH_GPU_CRASH_DEBUGGING=$<BOOL:${SPARKLE_WITH_GPU_CRASH_DEBUGGING}>)

# RHI publicly exposes Core-backed utility types only. Platform remains an
# implementation detail behind forward-declared window/timer types.
target_link_libraries(SparkleRHI
    PUBLIC
        SparkleRHIContracts
    PRIVATE
        SparklePlatform
        imgui
        ${SPARKLE_RHI_BACKEND_TARGETS}
)

# Keep the same target shape as NVIDIA NVRHI: neutral/common code and each
# native backend compile independently. These configure-time assertions make a
# misplaced source fail even when a combined D3D12+Vulkan build would link.
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHICommon D3D12)
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHICommon Vulkan)
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHIDiagnostics D3D12)
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHIDiagnostics Vulkan)
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHI D3D12)
sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHI Vulkan)
if(TARGET SparkleRHI_D3D12)
    sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHI_D3D12 Vulkan)
endif()
if(TARGET SparkleRHI_Vulkan)
    sparkle_assert_rhi_target_excludes_backend_sources(SparkleRHI_Vulkan D3D12)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_RHI_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private\\Common" FILES ${SPARKLE_RHI_COMMON_PRIVATE_SOURCES} ${SPARKLE_RHI_COMMON_PRIVATE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics PREFIX "Private\\Diagnostics" FILES ${SPARKLE_RHI_DIAGNOSTICS_SOURCES} ${SPARKLE_RHI_DIAGNOSTICS_HEADERS})

if(SPARKLE_RHI_WITH_D3D12)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12 PREFIX "Private\\D3D12" FILES ${SPARKLE_RHI_D3D12_SOURCES} ${SPARKLE_RHI_D3D12_HEADERS})
    source_group("ThirdParty\\D3D12MA" FILES ${SPARKLE_D3D12MA_SOURCES})
endif()

if(SPARKLE_RHI_WITH_VULKAN)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private/Vulkan PREFIX "Private\\Vulkan" FILES ${SPARKLE_RHI_VULKAN_SOURCES} ${SPARKLE_RHI_VULKAN_HEADERS})
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ExternalCapture.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/RuntimeDeployment.cmake")
