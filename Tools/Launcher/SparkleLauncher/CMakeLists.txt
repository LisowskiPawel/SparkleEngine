include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/LauncherCore.cmake")

find_package(Qt6 6.11.1 EXACT REQUIRED COMPONENTS Widgets)

set(SPARKLE_LAUNCHER_GUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationConfirmation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Build/LauncherBuildOptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Clean/LauncherCleanOptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Cook/LauncherCookOptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Scenes/LauncherSceneCards.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Scenes/LauncherSceneCatalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Scenes/LauncherSceneLaunch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Workspace/LauncherDependencyRows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Workspace/LauncherWorkspaceSetup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindowPages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherOptionWidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/GpuCrash/LauncherGpuCrashPanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherGuiApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationExecution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherProgressStreamDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherCleanUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherContextUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherExternalCaptureUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherDependencyUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherLevelUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherOperationRequestFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherContentModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherToolchainUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherUiModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherWorkflowCatalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherActivityPanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherWorkflowPanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindowActivity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindowOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindowShell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherIconLibrary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherUiDesign.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherVisualStyle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherActionWidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherArtworkWidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherSceneWidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherLayoutWidgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherSelectionWidgets.cpp
)

set(SPARKLE_LAUNCHER_GUI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationConfirmation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Build/LauncherBuildOptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Clean/LauncherCleanOptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Cook/LauncherCookOptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Scenes/LauncherSceneCatalog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Pages/Workspace/LauncherWorkspaceSetup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherOptionWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/GpuCrash/LauncherGpuCrashPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherBackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherGuiApp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationRequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationExecution.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationService.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherProgressStreamDecoder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherCleanUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherContextUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherExternalCaptureUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherDependencyUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherLevelUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherOperationResult.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherOperationRequestFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherContentModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherToolchainUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherUiModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Models/LauncherWorkflowCatalog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherActivityPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherWorkflowPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Shell/LauncherMainWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherIconLibrary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherUiDesign.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Style/LauncherVisualStyle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherActionWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherArtworkWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherSceneWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherLayoutWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Widgets/LauncherSelectionWidgets.h
)

set(SPARKLE_LAUNCHER_PRIVATE_SOURCES
    ${SPARKLE_LAUNCHER_GUI_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Startup/LauncherShadowExecution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellPresentation.cpp
)

set(SPARKLE_LAUNCHER_PRIVATE_HEADERS
    ${SPARKLE_LAUNCHER_GUI_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Startup/LauncherShadowExecution.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Startup/LauncherArguments.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShell.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellPresentation.h
)

add_executable(SparkleLauncher WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/main.cpp
    ${SPARKLE_LAUNCHER_PRIVATE_SOURCES}
    ${SPARKLE_LAUNCHER_PRIVATE_HEADERS}
)
sparkle_configure_host_tool_target(SparkleLauncher)

qt_add_resources(SparkleLauncher SparkleLauncherUiResources
    PREFIX "/SparkleLauncher"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Ui"
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Ui/footer-chevron.xpm
)

if(NOT DEFINED SPARKLE_FONT_AWESOME_SOLID_TTF OR NOT EXISTS "${SPARKLE_FONT_AWESOME_SOLID_TTF}")
    message(FATAL_ERROR "SparkleLauncher requires Font Awesome Free Solid. FetchDependencies.cmake should provide fa-solid-900.ttf.")
endif()
get_filename_component(SPARKLE_LAUNCHER_FONT_AWESOME_DIRECTORY "${SPARKLE_FONT_AWESOME_SOLID_TTF}" DIRECTORY)
qt_add_resources(SparkleLauncher SparkleLauncherFontResources
    PREFIX "/SparkleLauncher/Fonts"
    BASE "${SPARKLE_LAUNCHER_FONT_AWESOME_DIRECTORY}"
    FILES
        "${SPARKLE_FONT_AWESOME_SOLID_TTF}"
)

target_include_directories(SparkleLauncher
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(SparkleLauncher
    PRIVATE
        SparkleLauncherCore
        SparkleCore
        SparkleTasks
        ToolSupport
        Qt6::Widgets
)

target_compile_features(SparkleLauncher PRIVATE cxx_std_20)
sparkle_target_precompile_headers(SparkleLauncher
    <filesystem>
    <memory>
    <string>
    <vector>
    <QString>
    <QWidget>
)

# Cache the header-only parser only for its owning compilation unit.
add_library(SparkleLauncherCli OBJECT Private/Startup/LauncherArguments.cpp)
sparkle_configure_host_tool_target(SparkleLauncherCli)
target_include_directories(SparkleLauncherCli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(SparkleLauncherCli PRIVATE CLI11::CLI11 SparkleLauncherCore SparkleCore)
target_compile_features(SparkleLauncherCli PRIVATE cxx_std_20)
sparkle_target_precompile_headers(SparkleLauncherCli <CLI/CLI.hpp>)
target_link_libraries(SparkleLauncher PRIVATE SparkleLauncherCli)

if(MSVC)
    # Argument parsing is cold code; avoid duplicating the header-only SDK through inlining.
    target_compile_options(SparkleLauncherCli PRIVATE
            "$<$<CONFIG:DevelopmentEditor,ShippingEditor>:/O1>"
            "$<$<CONFIG:DevelopmentEditor,ShippingEditor>:/Ob0>")
endif()

set_target_properties(SparkleLauncher PROPERTIES
    AUTOMOC ON
)
sparkle_configure_launcher_artifacts(SparkleLauncher)
sparkle_configure_product_identity(SparkleLauncher "Sparkle Launcher")
sparkle_stage_linked_runtime(SparkleLauncher)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/LauncherDeployment.cmake")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES
    ${SPARKLE_LAUNCHER_PRIVATE_SOURCES} ${SPARKLE_LAUNCHER_PRIVATE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${CMAKE_CURRENT_SOURCE_DIR}/Private/main.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${CMAKE_CURRENT_SOURCE_DIR}/Private/Startup/LauncherArguments.cpp)
