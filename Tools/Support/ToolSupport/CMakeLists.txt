set(TOOL_SUPPORT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SourceAssetPaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/ToolConsole.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/ProjectLevelCatalog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/ToolOutputPaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/ToolWorkProgress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/ToolWorkProgressEvent.h
)

set(TOOL_SUPPORT_PRIVATE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/SourceAssetPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/ToolConsole.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelCatalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelSelectionWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelSelectionWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelCatalogReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelCatalogReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelCatalogValidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Projects/ProjectLevelCatalogValidator.h

    ${CMAKE_CURRENT_SOURCE_DIR}/Private/ToolOutputPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/ToolWorkProgress.cpp
)

add_library(ToolSupport STATIC
    ${TOOL_SUPPORT_PUBLIC_HEADERS}
    ${TOOL_SUPPORT_PRIVATE_SOURCES}
)
sparkle_configure_host_tool_target(ToolSupport)

target_include_directories(ToolSupport
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_compile_features(ToolSupport PUBLIC cxx_std_20)
target_link_libraries(ToolSupport PUBLIC SparkleCore)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${TOOL_SUPPORT_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${TOOL_SUPPORT_PRIVATE_SOURCES})

target_compile_definitions(ToolSupport PRIVATE "SPARKLE_ARTIFACT_VARIANT_NAME=\"${SPARKLE_ARTIFACT_VARIANT}\"")
