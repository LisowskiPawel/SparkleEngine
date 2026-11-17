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
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelSelectionWriter.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Levels/LevelSelectionWriter.h
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
sparkle_target_precompile_headers(SparkleLauncherCore
    <algorithm>
    <filesystem>
    <memory>
    <optional>
    <string>
    <string_view>
    <vector>
)

target_compile_definitions(SparkleLauncherCore
    PRIVATE
        SPARKLE_DIRECTXTEX_REVISION="${SPARKLE_DIRECTXTEX_REVISION}"
        SPARKLE_KTX_REVISION="${SPARKLE_KTX_REVISION}"
    PUBLIC
        SPARKLE_ENABLE_CONTENT_PIPELINE=$<BOOL:${SPARKLE_ENABLE_CONTENT_PIPELINE}>
        SPARKLE_ENABLE_SHADER_COMPILER=$<BOOL:${SPARKLE_ENABLE_SHADER_COMPILER}>
        SPARKLE_ENABLE_NVIDIA_STREAMLINE=$<BOOL:${SPARKLE_ENABLE_NVIDIA_STREAMLINE}>
)

target_link_libraries(SparkleLauncherCore
    PUBLIC SparkleCore SparkleRHIContracts
    PRIVATE ToolSupport nlohmann_json::nlohmann_json
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${SPARKLE_LAUNCHER_CORE_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES
    ${SPARKLE_LAUNCHER_CORE_FOUNDATION_SOURCES}
    ${SPARKLE_LAUNCHER_CORE_BUILD_SOURCES} ${SPARKLE_LAUNCHER_CORE_BUILD_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_LEVEL_SOURCES} ${SPARKLE_LAUNCHER_CORE_LEVEL_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_COOK_SOURCES} ${SPARKLE_LAUNCHER_CORE_COOK_HEADERS}
    ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_SOURCES} ${SPARKLE_LAUNCHER_CORE_MAINTENANCE_HEADERS})
