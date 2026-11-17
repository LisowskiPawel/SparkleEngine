file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

sparkle_add_engine_module(SparkleGameFramework SPARKLE_ENGINE_EXPORTS
        ${SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES}
        ${SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS}
    )

target_include_directories(SparkleGameFramework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        # Engine root for module-prefixed includes
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        # Core module subdirectories for direct includes
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Events
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Math
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Time
)

target_compile_features(SparkleGameFramework PUBLIC cxx_std_20)

# Precompiled header (per-module PCH)
sparkle_target_precompile_headers(SparkleGameFramework
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

target_link_libraries(SparkleGameFramework
    PUBLIC
        SparkleCore
		SparkleTasks
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_GAMEFRAMEWORK_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_GAMEFRAMEWORK_PRIVATE_SOURCES} ${SPARKLE_GAMEFRAMEWORK_PRIVATE_HEADERS})
