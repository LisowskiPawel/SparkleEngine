set(ASSET_COOKER_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Api/CookProjectContent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/AssetCookerSceneBatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookImportedScene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/BuildTextureCookRequests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/DefaultTextureCookRequestBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics/AssetCookerDiagnostics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Planning/ProjectCookPlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Dispatch/AssetCookerDispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookShaders.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookTextures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookSceneAssets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Dispatch/AssetCookerToolProcess.cpp
)

set(ASSET_COOKER_CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Api/CookProjectContent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Api/AssetCookerTypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/AssetCookerSceneBatch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookImportedScene.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/BuildTextureCookRequests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/DefaultTextureCookRequestBuilder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Diagnostics/AssetCookerDiagnostics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Planning/ProjectCookPlanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Dispatch/AssetCookerDispatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/AssetCookStages.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Dispatch/AssetCookerToolProcess.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Planning/ProjectCookPlan.h
)

set(ASSET_COOKER_CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/AssetCookerCli.cpp
)

set(ASSET_COOKER_CLI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/AssetCookerCli.h
)

add_library(AssetCookerCore STATIC
    ${ASSET_COOKER_CORE_SOURCES}
    ${ASSET_COOKER_CORE_HEADERS}
)
sparkle_configure_host_tool_target(AssetCookerCore)

target_include_directories(AssetCookerCore
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_SOURCE_DIR}/Engine
        ${CMAKE_SOURCE_DIR}/Engine/Core/Public/Diagnostics
        ${CMAKE_SOURCE_DIR}/Projects/Showcase/AssetPreparation
)

target_compile_features(AssetCookerCore PRIVATE cxx_std_20)
set(ASSET_COOKER_COOKING_LINK_LIBRARIES
    SourceImporters
    MeshCooker
    MaterialCooker
    SceneCooker
    TextureCookShared
    ToolSupport
    SparkleCore
    SparkleGameFramework
    enkiTS
)

target_link_libraries(AssetCookerCore
    PRIVATE
        ${ASSET_COOKER_COOKING_LINK_LIBRARIES}
)

add_executable(AssetCooker
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/main.cpp
    ${ASSET_COOKER_CLI_SOURCES}
    ${ASSET_COOKER_CLI_HEADERS}
)
sparkle_configure_host_tool_target(AssetCooker)
sparkle_configure_development_tool_artifacts(AssetCooker)
sparkle_stage_msvc_runtime(AssetCooker)

target_include_directories(AssetCooker
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_SOURCE_DIR}/Engine
        ${CMAKE_SOURCE_DIR}/Engine/Core/Public/Diagnostics
)

target_compile_features(AssetCooker PRIVATE cxx_std_20)
sparkle_target_precompile_headers(AssetCooker <CLI/CLI.hpp>)
if(MSVC)
    # Bound CLI11 parser/help code size without changing the cooking libraries.
    target_compile_options(AssetCooker PRIVATE
        "$<$<CONFIG:DevelopmentEditor,DevelopmentGame,ShippingEditor,ShippingGame>:/O1>"
        "$<$<CONFIG:DevelopmentEditor,DevelopmentGame,ShippingEditor,ShippingGame>:/Ob0>")
endif()
target_link_libraries(AssetCooker
    PRIVATE
        CLI11::CLI11
        AssetCookerCore
        ${ASSET_COOKER_COOKING_LINK_LIBRARIES}
)
sparkle_stage_linked_runtime(AssetCooker)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PREFIX "Private"
    FILES ${ASSET_COOKER_CORE_SOURCES} ${ASSET_COOKER_CORE_HEADERS} ${ASSET_COOKER_CLI_SOURCES} ${ASSET_COOKER_CLI_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${CMAKE_CURRENT_SOURCE_DIR}/Private/main.cpp)
