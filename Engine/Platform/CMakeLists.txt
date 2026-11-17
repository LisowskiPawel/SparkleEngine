# SparklePlatform Module
# Native window and input ownership
# Dependencies: SparkleCore

file(GLOB_RECURSE SPARKLE_PLATFORM_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_PLATFORM_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_PLATFORM_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

sparkle_add_engine_module(SparklePlatform SPARKLE_PLATFORM_EXPORTS
        ${SPARKLE_PLATFORM_PUBLIC_HEADERS}
        ${SPARKLE_PLATFORM_PRIVATE_SOURCES}
        ${SPARKLE_PLATFORM_PRIVATE_HEADERS}
    )

target_include_directories(SparklePlatform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        # Engine root for module-prefixed includes (e.g., "Platform/...")
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(SparklePlatform PUBLIC cxx_std_20)

# Precompiled header (per-module PCH)
sparkle_target_precompile_headers(SparklePlatform
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

# Platform depends on Core
target_link_libraries(SparklePlatform
    PUBLIC
        SparkleCore
    PRIVATE
        dwmapi
)

sparkle_configure_runtime_support_artifacts(SparklePlatform)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_PLATFORM_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_PLATFORM_PRIVATE_SOURCES} ${SPARKLE_PLATFORM_PRIVATE_HEADERS})
