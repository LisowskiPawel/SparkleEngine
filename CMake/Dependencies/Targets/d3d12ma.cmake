sparkle_declare_git_dependency(sparkle_d3d12ma
    https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
    0fa62ed3a0a69b73230a8ec1faa752d4061c8dc8
    ID d3d12ma VERSION "0fa62ed3a0a69b73230a8ec1faa752d4061c8dc8" PACKAGE_NAME "D3D12MemoryAllocator" LICENSE_ID MIT
    REQUIRED_PATHS src/D3D12MemAlloc.cpp include/D3D12MemAlloc.h LICENSE.txt NOTICES.txt)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(SparkleD3D12MA STATIC
    ${sparkle_d3d12ma_SOURCE_DIR}/src/D3D12MemAlloc.cpp
    ${sparkle_d3d12ma_SOURCE_DIR}/include/D3D12MemAlloc.h)

target_include_directories(SparkleD3D12MA
    SYSTEM PUBLIC
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

set_target_properties(SparkleD3D12MA PROPERTIES FOLDER "ThirdParty/D3D12MemoryAllocator")
sparkle_add_dependency_source_package(SparkleD3D12MA d3d12ma)
sparkle_add_bundled_source_package(SparkleD3D12MA d3d12ma "D3D12MA adapted Microsoft graphics samples" MIT NOTICES.txt)
sparkle_add_bundled_source_package(SparkleD3D12MA d3d12ma "D3D12MA adapted VMA code" MIT NOTICES.txt)

