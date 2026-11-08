set(SPARKLE_LAUNCHER_CORE_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/GpuCrashIncident.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/BuildWorkspaceOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/ArtifactNaming.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/BuildProfileCatalog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/EditorShaderCompilation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/CookOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/LauncherContentDefaults.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/LevelOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/LevelRunOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/MaintenanceOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/OperationModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/ProcessRunner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/ProjectResolver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/WorkspaceLocator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/SourceDependencyState.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/SparkleLauncher/ToolResolver.h
)

set(SPARKLE_LAUNCHER_CORE_FOUNDATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildProfileCatalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Editor/EditorShaderCompilation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherRootResolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherRootResolver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Operations/LauncherOperationProgress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Operations/LauncherOperationProgress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherStatePaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherStatePaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Operations/OperationModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Operations/ProcessRunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/ProjectResolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherProductPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/LauncherProductPaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Workspace/WorkspaceLocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Dependencies/SourceDependencyState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/HostGraphicsCapabilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/ToolResolver.cpp
)

set(SPARKLE_LAUNCHER_CORE_BUILD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/State/BuildFilesFreshness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/State/BuildFreshnessSignature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/NativeBuildOutputReset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/BuildToolchainDetection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/HostToolInstaller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/QtToolchainDiscovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/ShaderCompilerSdkDiscovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/VisualStudioToolchainDiscovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/VulkanSdkDiscovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceFailure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspacePlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceProcessRequests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/State/BuildWorkspaceStateFiles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/CMakeGeneratorModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/CMakeWorkflowProcessRequests.cpp
)

set(SPARKLE_LAUNCHER_CORE_BUILD_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/State/BuildFreshnessSignature.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/NativeBuildOutputReset.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceProcessRequests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/BuildWorkspaceFailure.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/State/BuildWorkspaceStateFiles.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/CMakeGeneratorModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/CMakeWorkflowProcessRequests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/HostToolInstaller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/QtToolchainDiscovery.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/ShaderCompilerSdkDiscovery.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/VisualStudioToolchainDiscovery.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Build/Toolchains/VulkanSdkDiscovery.h
)

set(SPARKLE_LAUNCHER_CORE_LEVEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/GpuCrash/GpuCrashLaunch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/GpuCrash/GpuCrashLaunch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/AssetPackSyncPlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelPreparation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelCleanPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelSelectionCommit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelOperationExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelOperationProcessRequests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelRunOperationExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelRunOperationProcessRequests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelRunOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/ExternalCapture/ExternalCaptureDiscovery.cpp
)

set(SPARKLE_LAUNCHER_CORE_LEVEL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/AssetPackSyncPlanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelPreparation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelSelectionCommit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelOperationProcessRequests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelRunOperationProcessRequests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/ExternalCapture/ExternalCaptureDiscovery.h
)

set(SPARKLE_LAUNCHER_CORE_COOK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookedContentReadiness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookOperationExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookOperationProcessRequests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookOperationPlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookPlanReadiness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookPlanPreview.cpp
)

set(SPARKLE_LAUNCHER_CORE_COOK_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookPlanReadiness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookPlanPreview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookedContentReadiness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cook/CookOperationProcessRequests.h
)

set(SPARKLE_LAUNCHER_CORE_MAINTENANCE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceOperationExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanSteps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceOperationPlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanPreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanExecution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/ActionCleanPaths.cpp
)

set(SPARKLE_LAUNCHER_CORE_MAINTENANCE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanSteps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanPreview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Maintenance/MaintenanceCleanExecution.h
)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/GpuCrash/LauncherGpuCrashPanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/GpuCrash/LauncherGpuCrashPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherGuiApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationRequestMapping.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherBackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherGuiApp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/App/LauncherSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationRequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Gui/Operations/LauncherOperationRequestMapping.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellArguments.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellPresentation.cpp
)

set(SPARKLE_LAUNCHER_PRIVATE_HEADERS
    ${SPARKLE_LAUNCHER_GUI_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Startup/LauncherShadowExecution.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellArguments.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShell.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellOperations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellPresentation.h
)

add_library(SparkleLauncherCore STATIC
    ${SPARKLE_LAUNCHER_CORE_PUBLIC_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_FOUNDATION_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_BUILD_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_BUILD_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_LEVEL_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_LEVEL_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_COOK_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_COOK_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_HEADERS}
)
sparkle_configure_host_tool_target(SparkleLauncherCore)
sparkle_configure_launcher_artifacts(SparkleLauncherCore)

target_include_directories(SparkleLauncherCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_compile_features(SparkleLauncherCore PUBLIC cxx_std_20)

target_compile_definitions(SparkleLauncherCore
    PUBLIC
        SPARKLE_ENABLE_CONTENT_PIPELINE=$<BOOL:${SPARKLE_ENABLE_CONTENT_PIPELINE}>
        SPARKLE_ENABLE_SHADER_COMPILER=$<BOOL:${SPARKLE_ENABLE_SHADER_COMPILER}>
        SPARKLE_ENABLE_KTX_SUPPORT=$<BOOL:${SPARKLE_ENABLE_KTX_SUPPORT}>
        SPARKLE_ENABLE_NVIDIA_STREAMLINE=$<BOOL:${SPARKLE_ENABLE_NVIDIA_STREAMLINE}>
)

target_link_libraries(SparkleLauncherCore
    PUBLIC SparkleCore SparkleRHIContracts
    PRIVATE ToolSupport
)

add_executable(SparkleLauncher WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp
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
        CLI11::CLI11
        SparkleLauncherCore
        SparkleCore
        SparkleTasks
        ToolSupport
        Qt6::Widgets
)

target_compile_features(SparkleLauncher PRIVATE cxx_std_20)
target_compile_definitions(SparkleLauncher PRIVATE CLI11_ENABLE_EXTRA_VALIDATORS=0)

if(MSVC)
    # Argument parsing is cold code; avoid duplicating the header-only SDK through inlining.
    set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shell/LauncherShellArguments.cpp
        APPEND PROPERTY COMPILE_OPTIONS
            "$<$<CONFIG:DevelopmentEditor,ShippingEditor>:/O1>"
            "$<$<CONFIG:DevelopmentEditor,ShippingEditor>:/Ob0>")
endif()

set_target_properties(SparkleLauncher PROPERTIES
    AUTOMOC ON
)
sparkle_configure_launcher_artifacts(SparkleLauncher)
sparkle_configure_product_identity(SparkleLauncher "Sparkle Launcher")
sparkle_declare_runtime_dll_owner(SparkleLauncher SparkleCore SparkleTasks)

add_custom_command(TARGET SparkleLauncher POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Visuals"
        "$<TARGET_FILE_DIR:SparkleLauncher>/Visuals"
    COMMENT "Deploying SparkleLauncher visual artwork"
)

if(WIN32)
    get_target_property(SPARKLE_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(SPARKLE_QT_BIN_DIR "${SPARKLE_QMAKE_EXECUTABLE}" DIRECTORY)

    find_program(SPARKLE_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${SPARKLE_QT_BIN_DIR}"
        REQUIRED
    )

    set(_sparkle_qt_deploy_command "${SPARKLE_WINDEPLOYQT_EXECUTABLE}")
    if(CMAKE_GENERATOR MATCHES "^Visual Studio" AND CMAKE_GENERATOR_INSTANCE)
        string(REGEX REPLACE ",.*$" "" _sparkle_visual_studio_root "${CMAKE_GENERATOR_INSTANCE}")
        set(_sparkle_qt_deploy_command
            "${CMAKE_COMMAND}" -E env "VCINSTALLDIR=${_sparkle_visual_studio_root}/VC/"
            "${SPARKLE_WINDEPLOYQT_EXECUTABLE}")
    endif()

    set(_sparkle_qt_notice_files
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/README.md"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/ThirdPartyNotices.txt"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/LGPL-3.0-only.txt"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/GPL-3.0-only.txt")
    foreach(_notice IN LISTS _sparkle_qt_notice_files)
        if(NOT EXISTS "${_notice}")
            message(FATAL_ERROR "Qt deployment notice is missing: '${_notice}'")
        endif()
    endforeach()
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
        CONTENT "${Qt6_VERSION}\n")

    add_custom_target(SparkleLauncherQtNotices
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt"
            "$<TARGET_FILE_DIR:SparkleLauncher>/Notices/Qt"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
            "$<TARGET_FILE_DIR:SparkleLauncher>/Notices/Qt/QtRuntimeVersion.txt"
        DEPENDS ${_sparkle_qt_notice_files} "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
        COMMENT "Deploying Qt notices"
        VERBATIM
    )
    add_dependencies(SparkleLauncher SparkleLauncherQtNotices)

    add_custom_command(TARGET SparkleLauncher POST_BUILD
        COMMAND ${_sparkle_qt_deploy_command}
            --verbose 0
            --no-compiler-runtime
            --dir "$<TARGET_FILE_DIR:SparkleLauncher>"
            "$<TARGET_FILE:SparkleLauncher>"
        COMMENT "Deploying Qt runtime for SparkleLauncher artifact"
    )
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_LAUNCHER_CORE_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SPARKLE_LAUNCHER_CORE_FOUNDATION_SOURCES} ${SPARKLE_LAUNCHER_CORE_BUILD_SOURCES} ${SPARKLE_LAUNCHER_CORE_BUILD_HEADERS} ${SPARKLE_LAUNCHER_CORE_LEVEL_SOURCES} ${SPARKLE_LAUNCHER_CORE_LEVEL_HEADERS} ${SPARKLE_LAUNCHER_CORE_COOK_SOURCES} ${SPARKLE_LAUNCHER_CORE_COOK_HEADERS} ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_SOURCES} ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_HEADERS} ${SPARKLE_LAUNCHER_PRIVATE_SOURCES} ${SPARKLE_LAUNCHER_PRIVATE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "Source" FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp)
