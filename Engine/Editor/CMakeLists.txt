file(GLOB_RECURSE SPARKLE_EDITOR_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
)

file(GLOB_RECURSE SPARKLE_EDITOR_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SPARKLE_EDITOR_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

set(_sparkle_editor_capture_sources ${SPARKLE_EDITOR_PRIVATE_SOURCES})
list(FILTER _sparkle_editor_capture_sources INCLUDE REGEX "/Private/Viewport/ExternalCapture/")
list(FILTER SPARKLE_EDITOR_PRIVATE_SOURCES EXCLUDE REGEX "/Private/Viewport/ExternalCapture/")

sparkle_add_engine_module(SparkleEditor SPARKLE_EDITOR_EXPORTS
        ${SPARKLE_EDITOR_PUBLIC_HEADERS}
        ${SPARKLE_EDITOR_PRIVATE_SOURCES}
        ${SPARKLE_EDITOR_PRIVATE_HEADERS}
    )

target_include_directories(SparkleEditor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/ImGui
        # Engine root for module-prefixed includes
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        # Core module subdirectories for direct includes
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Time
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
)

target_compile_features(SparkleEditor PUBLIC cxx_std_20)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/EditorIconAssets.cmake")

set(SPARKLE_EDITOR_CAPTURE_UI_CONFIG "$<OR:$<CONFIG:DebugEditor>,$<CONFIG:DevelopmentEditor>>")
foreach(_source IN LISTS _sparkle_editor_capture_sources)
    target_sources(SparkleEditor PRIVATE "$<${SPARKLE_EDITOR_CAPTURE_UI_CONFIG}:${_source}>")
endforeach()

# Precompiled header (per-module PCH)
sparkle_target_precompile_headers(SparkleEditor
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

# Editor exposes Core, Renderer/world contracts and the RHI capture contract.
# Platform and ImGui stay implementation dependencies.
target_link_libraries(SparkleEditor
    PUBLIC
        SparkleCore
        SparkleRHI
        SparkleRenderer
        SparkleGameFramework
    PRIVATE
        nlohmann_json::nlohmann_json
        SparklePlatform
        imgui
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_EDITOR_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_EDITOR_PRIVATE_SOURCES} ${SPARKLE_EDITOR_PRIVATE_HEADERS})
