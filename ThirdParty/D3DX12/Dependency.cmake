# The historical vendor header is byte-pinned separately from Sparkle's wrapper.
sparkle_collect_missing_relative_paths("${CMAKE_CURRENT_LIST_DIR}" missing_vendor_inputs
    Include/d3dx12.h LICENSE.txt README.md)
if(missing_vendor_inputs)
    message(FATAL_ERROR "D3DX12 vendor/origin/license inputs are incomplete: ${missing_vendor_inputs}")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Include/d3dx12.h")
file(SHA256 "${CMAKE_CURRENT_LIST_DIR}/Include/d3dx12.h" _d3dx12_hash)
if(NOT _d3dx12_hash STREQUAL "bc48e96fe3995e8181a5ecd1cab5840b095dba58044523abacbacb52fd7ead2b")
    message(FATAL_ERROR "D3DX12 differs from its recorded vendor identity; review and update the origin record.")
endif()
add_library(SparkleD3DX12 INTERFACE)
target_include_directories(SparkleD3DX12 SYSTEM INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Include")
sparkle_add_source_package(SparkleD3DX12 "D3DX12" "sha256:${_d3dx12_hash}"
    "MIT" "https://github.com/microsoft/DirectX-Headers")
set_target_properties(SparkleD3DX12 PROPERTIES FOLDER "ThirdParty/D3DX12")
