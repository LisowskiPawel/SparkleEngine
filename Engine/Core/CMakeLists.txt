# SparkleCore Module
# Foundation module: Math, Memory, Containers, Logging, Events, Time, Input types

file(GLOB_RECURSE SPARKLE_CORE_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_CORE_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_CORE_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

sparkle_add_engine_module(SparkleCore SPARKLE_CORE_EXPORTS
        ${SPARKLE_CORE_PUBLIC_HEADERS}
        ${SPARKLE_CORE_PRIVATE_SOURCES}
        ${SPARKLE_CORE_PRIVATE_HEADERS}
    )

# Set include directories
# PUBLIC: Headers that consumers of this module need
# PRIVATE: Headers only used internally (plus Public for internal compilation)
target_include_directories(SparkleCore
    PUBLIC
        # Engine root for module-prefixed includes (e.g., "Core/...")
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        # Subdirectories for easier includes
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Math
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Events
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Diagnostics
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Hash
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Strings
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Time
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Input
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Input
)

# Require C++20
target_compile_features(SparkleCore PUBLIC cxx_std_20)

# Precompiled header (per-module PCH)
sparkle_target_precompile_headers(SparkleCore
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

# Core module has no engine dependencies (it's the base)
# Add any system dependencies here if needed
# SparkleCore owns spdlog bootstrap, sink policy, and named logger lifetime.
target_link_libraries(SparkleCore
    PUBLIC
        spdlog::spdlog
    PRIVATE
        Boost::process
)

if(WIN32)
    target_link_libraries(SparkleCore PRIVATE bcrypt ole32 shell32)
endif()

target_compile_definitions(SparkleCore
    PUBLIC
    $<$<CONFIG:DebugEditor,DebugGame>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:DevelopmentEditor,DevelopmentGame>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
    $<$<CONFIG:ShippingEditor,ShippingGame>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN>
)

# IDE organization
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_CORE_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_CORE_PRIVATE_SOURCES} ${SPARKLE_CORE_PRIVATE_HEADERS})

target_compile_definitions(SparkleCore PRIVATE "SPARKLE_ARTIFACT_VARIANT_NAME=\"${SPARKLE_ARTIFACT_VARIANT}\"")
