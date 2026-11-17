if(SPARKLE_RHI_WITH_D3D12_NVAPI)
    if(SPARKLE_RHI_D3D12_NVAPI_FETCH_FROM_GITHUB)
        sparkle_populate_nvidia_nvapi()

        set(SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR "${sparkle_nvapi_SOURCE_DIR}" CACHE PATH "Directory containing NVIDIA NVAPI headers, used only by SparkleRHI_D3D12" FORCE)
        set(SPARKLE_RHI_D3D12_NVAPI_LIBRARY "${sparkle_nvapi_SOURCE_DIR}/amd64/nvapi64.lib" CACHE FILEPATH "NVIDIA NVAPI import library, used only by SparkleRHI_D3D12" FORCE)
    endif()

    if(NOT SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR)
        message(FATAL_ERROR "SPARKLE_RHI_WITH_D3D12_NVAPI is ON, but SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR is empty.")
    endif()
    if(NOT SPARKLE_RHI_D3D12_NVAPI_LIBRARY)
        message(FATAL_ERROR "SPARKLE_RHI_WITH_D3D12_NVAPI is ON, but SPARKLE_RHI_D3D12_NVAPI_LIBRARY is empty.")
    endif()
    if(NOT EXISTS "${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}/nvapi.h")
        message(FATAL_ERROR "SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR does not contain nvapi.h: ${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}")
    endif()
    if(NOT EXISTS "${SPARKLE_RHI_D3D12_NVAPI_LIBRARY}")
        message(FATAL_ERROR "SPARKLE_RHI_D3D12_NVAPI_LIBRARY does not exist: ${SPARKLE_RHI_D3D12_NVAPI_LIBRARY}")
    endif()
endif()

sparkle_declare_git_dependency(sparkle_d3d12ma
    https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
    0fa62ed3a0a69b73230a8ec1faa752d4061c8dc8)

FetchContent_GetProperties(sparkle_d3d12ma)
if(NOT sparkle_d3d12ma_POPULATED)
    FetchContent_Populate(sparkle_d3d12ma)
endif()

set(SPARKLE_D3D12MA_SOURCES
    ${sparkle_d3d12ma_SOURCE_DIR}/src/D3D12MemAlloc.cpp
    ${sparkle_d3d12ma_SOURCE_DIR}/include/D3D12MemAlloc.h
)

add_library(SparkleD3D12MA STATIC ${SPARKLE_D3D12MA_SOURCES})

target_include_directories(SparkleD3D12MA
    PUBLIC
        ${sparkle_d3d12ma_SOURCE_DIR}/include
)

target_compile_features(SparkleD3D12MA PUBLIC cxx_std_14)

target_link_libraries(SparkleD3D12MA
    PUBLIC
        d3d12
        dxgi
        dxguid
)

if(MSVC)
    target_compile_options(SparkleD3D12MA PRIVATE /W0)
endif()

set_target_properties(SparkleD3D12MA PROPERTIES FOLDER "ThirdParty")

file(GLOB_RECURSE SPARKLE_RHI_D3D12_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12/*.cpp
)

file(GLOB_RECURSE SPARKLE_RHI_D3D12_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12/*.h
)

if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    list(FILTER SPARKLE_RHI_D3D12_SOURCES EXCLUDE REGEX "/Diagnostics/GpuCrash/")
    list(FILTER SPARKLE_RHI_D3D12_HEADERS EXCLUDE REGEX "/Diagnostics/GpuCrash/")
endif()

set(SPARKLE_RHI_D3D12_THIRD_PARTY_SOURCES
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
)

list(FILTER SPARKLE_RHI_D3D12_SOURCES EXCLUDE REGEX "/D3D12PixEvents\\.cpp$")

set(_sparkle_d3d12_capture_sources ${SPARKLE_RHI_D3D12_SOURCES})
list(FILTER _sparkle_d3d12_capture_sources INCLUDE REGEX "/Diagnostics/ExternalCapture/")
list(FILTER SPARKLE_RHI_D3D12_SOURCES EXCLUDE REGEX "/Diagnostics/ExternalCapture/")
add_library(SparkleRHI_D3D12 STATIC
    ${SPARKLE_RHI_D3D12_SOURCES}
    ${SPARKLE_RHI_D3D12_HEADERS}
    ${SPARKLE_RHI_D3D12_THIRD_PARTY_SOURCES}
)

sparkle_configure_rhi_implementation_target(SparkleRHI_D3D12)

target_link_libraries(SparkleRHI_D3D12
    PRIVATE
        SparkleCore
        imgui
        SparkleD3D12MA
        d3d12
        dxgi
        d3dcompiler
        dxguid
    PRIVATE
        SparklePlatform
        SparkleRHIDiagnostics
)

target_compile_definitions(SparkleRHI_D3D12
    PRIVATE
        SPARKLE_RHI_WITH_D3D12_NVAPI=$<BOOL:${SPARKLE_RHI_WITH_D3D12_NVAPI}>
)

set(_sparkle_pix_enabled "0")
if(SPARKLE_RHI_WITH_D3D12_PIX_EVENTS)
    if(NOT TARGET Microsoft::WinPixEventRuntime)
        message(FATAL_ERROR "Official WinPixEventRuntime is required when D3D12 PIX events are selected.")
    endif()
    string(JOIN "," _sparkle_pix_configurations ${SPARKLE_DEBUG_CONFIGURATIONS} ${SPARKLE_DEVELOPMENT_CONFIGURATIONS})
    set(_sparkle_pix_enabled "$<CONFIG:${_sparkle_pix_configurations}>")
    target_sources(SparkleRHI_D3D12 PRIVATE
        "$<${_sparkle_pix_enabled}:${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12/Diagnostics/D3D12PixEvents.cpp>")
    target_link_libraries(SparkleRHI_D3D12 PRIVATE "$<${_sparkle_pix_enabled}:Microsoft::WinPixEventRuntime>")
    target_compile_definitions(SparkleRHI_D3D12 PRIVATE "$<${_sparkle_pix_enabled}:USE_PIX>")
endif()
set_property(TARGET SparkleRHI_D3D12 PROPERTY SPARKLE_PIX_EVENTS_ENABLED "${_sparkle_pix_enabled}")
target_compile_definitions(SparkleRHI_D3D12 PRIVATE SPARKLE_RHI_WITH_D3D12_PIX_EVENTS=${_sparkle_pix_enabled})

if(SPARKLE_RHI_WITH_D3D12_NVAPI)
    target_include_directories(SparkleRHI_D3D12
        PRIVATE
            ${SPARKLE_RHI_D3D12_NVAPI_INCLUDE_DIR}
    )
    target_link_libraries(SparkleRHI_D3D12
        PRIVATE
            ${SPARKLE_RHI_D3D12_NVAPI_LIBRARY}
    )
endif()

sparkle_target_precompile_headers(SparkleRHI_D3D12
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12/D3D12PCH.h
)

list(APPEND SPARKLE_RHI_BACKEND_TARGETS SparkleRHI_D3D12)
