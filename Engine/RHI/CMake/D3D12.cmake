include("${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/Targets/d3dx12.cmake")

if(SPARKLE_RHI_WITH_D3D12_NVAPI)
    sparkle_configure_nvidia_nvapi_target()
endif()

include("${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/Targets/d3d12ma.cmake")

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

add_library(imgui_d3d12 STATIC ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)
target_link_libraries(imgui_d3d12 PUBLIC imgui d3d12 dxgi d3dcompiler)
set_target_properties(imgui_d3d12 PROPERTIES FOLDER "ThirdParty/ImGui")

list(FILTER SPARKLE_RHI_D3D12_SOURCES EXCLUDE REGEX "/D3D12PixEvents\\.cpp$")

set(_sparkle_d3d12_capture_sources ${SPARKLE_RHI_D3D12_SOURCES})
list(FILTER _sparkle_d3d12_capture_sources INCLUDE REGEX "/Diagnostics/ExternalCapture/")
list(FILTER SPARKLE_RHI_D3D12_SOURCES EXCLUDE REGEX "/Diagnostics/ExternalCapture/")
add_library(SparkleRHI_D3D12 STATIC
    ${SPARKLE_RHI_D3D12_SOURCES}
    ${SPARKLE_RHI_D3D12_HEADERS}
)

sparkle_configure_rhi_implementation_target(SparkleRHI_D3D12)

target_link_libraries(SparkleRHI_D3D12
    PRIVATE
        SparkleCore
        imgui
        imgui_d3d12
        SparkleD3D12MA
        SparkleD3DX12
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
    target_link_libraries(SparkleRHI_D3D12 PRIVATE SparkleNVAPI)
endif()

sparkle_target_precompile_headers(SparkleRHI_D3D12
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/D3D12/D3D12PCH.h
)

list(APPEND SPARKLE_RHI_BACKEND_TARGETS SparkleRHI_D3D12)
