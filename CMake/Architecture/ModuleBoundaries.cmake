function(sparkle_check_module_boundaries _relative_path _lines)
    if(_relative_path MATCHES "^Engine/Editor/Public/(Console/EditorConsoleSystem|Shaders/RegisteredShaderListModel|Settings/EditorRestartService)[.]h$")
        sparkle_boundary_append_failure(
            "EDITOR_IMPLEMENTATIONS_REMAIN_PRIVATE"
            "${_relative_path}"
            "1"
            "Console controllers, panel models and restart implementations are private; hosts consume bounded UI operations."
            "${_relative_path}")
    endif()

    if(_relative_path MATCHES "^Engine/Editor/Public/Panels/")
        sparkle_boundary_append_failure(
            "EDITOR_PANELS_REMAIN_PRIVATE"
            "${_relative_path}"
            "1"
            "Editor panel implementations are private; the public surface may expose bounded host intents, not panel classes."
            "${_relative_path}")
    endif()

    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(_relative_path MATCHES "^Engine/" AND NOT _relative_path MATCHES "^Engine/Editor/" AND
           _line MATCHES "EditorConsoleSystem|GetEditorConsoleSystem")
            sparkle_boundary_append_failure(
                "EDITOR_CONSOLE_OWNER_STAYS_PRIVATE"
                "${_relative_path}"
                "${_line_number}"
                "Hosts publish console records through UI; they do not obtain the Editor console controller."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Editor/" AND _line MATCHES "CreateProcessW|CommandLineToArgvW|GetCommandLineW")
            sparkle_boundary_append_failure(
                "EDITOR_NO_PROCESS_LAUNCH_MECHANICS"
                "${_relative_path}"
                "${_line_number}"
                "Editor presents restart intent; Application owns close policy and Core owns process launch mechanics."
                "${_line}")
        endif()

        if(NOT _relative_path MATCHES "^Engine/Platform/Private/Window/(Window\\.cpp|WindowDisplay\\.cpp|SparkleApplication\\.manifest)$" AND
           _line MATCHES "${SPARKLE_BOUNDARY_NATIVE_DPI_POLICY_REGEX}")
            sparkle_boundary_append_failure(
                "WINDOW_DPI_POLICY_REMAINS_PLATFORM_OWNED"
                "${_relative_path}"
                "${_line_number}"
                "The shared application manifest owns process DPI mode, Platform Window owns native DPI changes, and the Win32 ImGui backend owns UI client extent."
                "${_line}")
        endif()

        if(NOT _relative_path MATCHES "^Engine/Renderer/" AND
           _line MATCHES "#include[^\n]*Renderer/Private/")
            sparkle_boundary_append_failure(
                "RENDERER_PRIVATE_NO_EXTERNAL_CONSUMERS"
                "${_relative_path}"
                "${_line_number}"
                "Other modules must consume Renderer/Public contracts and may not reach into Renderer implementation headers."
                "${_line}")
        endif()

        if(NOT _relative_path STREQUAL "Engine/Renderer/Private/Host/RendererHost.cpp" AND
           _line MATCHES "(make_unique<FramePipeline>|new[ \t]+FramePipeline)")
            sparkle_boundary_append_failure(
                "RENDERER_FRAME_PIPELINE_CONSTRUCTION_REMAINS_HOST_OWNED"
                "${_relative_path}"
                "${_line_number}"
                "RendererHost is the sole composition point for FramePipeline implementation dependencies."
                "${_line}")
        endif()

        if(NOT _relative_path MATCHES "^Engine/Renderer/Private/Host/Renderer(BackendOwner|Host)[.](cpp|h)$" AND
           _line MATCHES "#include[^\n]*Host/RendererBackendOwner[.]h")
            sparkle_boundary_append_failure(
                "RENDERER_BACKEND_OWNER_REMAINS_HOST_PRIVATE"
                "${_relative_path}"
                "${_line_number}"
                "RendererBackendOwner is a RendererHost implementation detail and may not become a parallel backend access route."
                "${_line}")
        endif()

        if(NOT _relative_path MATCHES "^Engine/Renderer/Private/(Frame/FramePipeline[.]cpp|Passes/Scene/SceneRenderingPasses[.]cpp|Passes/Lighting/ReferencePathTracer/)" AND
           NOT _relative_path STREQUAL "Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer/ReferencePathTracerShaderRegistrations.cpp" AND
           _line MATCHES "#include[^\n]*Passes/Lighting/ReferencePathTracer/")
            sparkle_boundary_append_failure(
                "REFERENCE_PATH_TRACER_CPP_CAPSULE"
                "${_relative_path}"
                "${_line_number}"
                "Reference Path Tracer implementation headers remain inside the feature, the scene-rendering composition and frame-lifecycle owners, and shader registration."
                "${_line}")
        endif()

        if((_relative_path MATCHES "^Engine/(Renderer|Editor)/" OR
            _relative_path MATCHES "^Engine/GameFramework/Public/") AND
           _line MATCHES "#include[^\n]*(World/ECS|GameFramework/Private/World)")
            sparkle_boundary_append_failure(
                "ECS_STORAGE_REMAINS_GAMEFRAMEWORK_PRIVATE"
                "${_relative_path}"
                "${_line_number}"
                "Renderer, Editor, and GameFramework public contracts may consume stable world capabilities, never private ECS storage."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Private/Frame/Graph/BuildRenderFrameGraph.cpp" AND
           _line MATCHES "Passes/.+Pass\\.h" AND
           NOT _line MATCHES "Passes/Scene/RayTracingScenePass[.]h")
            sparkle_boundary_append_failure(
                "FRAME_ORCHESTRATOR_NO_PASS_IMPLEMENTATIONS"
                "${_relative_path}"
                "${_line_number}"
                "The frame orchestrator must call subsystem-level frame APIs rather than concrete pass implementations."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/" AND
           _line MATCHES "ProjectLevelCatalog|ToolOutputPaths|LauncherProductPaths|SourceAssetPaths|SourceAssetType|SourceAssetRoot|ImportedTextureCacheRoot|ShaderDependencyManifestPath|ResolveAssetPath|GetTypedPath|GetShaderPath|texture-cooker-summary|texturecook-summary")
            sparkle_boundary_append_failure(
                "ENGINE_NO_PREPARATION_POLICY"
                "${_relative_path}"
                "${_line_number}"
                "Build outputs, acquisition catalogs and Launcher product policy belong to tools; Engine consumes ordinary runtime inputs."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/" AND
           (_line MATCHES "^[ \t]*#[ \t]*include[ \t]*[<\"].*(Tools/|SparkleLauncher/|ToolConsole[.]h|ToolWorkProgress|ShaderCompiler/)" OR
            (_relative_path MATCHES "(/CMakeLists[.]txt|[.]cmake)$" AND
             _line MATCHES "(^|[ \t;(])(ToolSupport|SparkleLauncher(Core)?|ShaderCompiler|AssetCooker|TextureCooker|SceneCooker|MaterialCooker|MeshCooker|SourceImporters)([ \t;)]|$)")))
            sparkle_boundary_append_failure(
                "ENGINE_NO_HOST_TOOL_DEPENDENCY"
                "${_relative_path}"
                "${_line_number}"
                "Host-tool includes and link dependencies belong to tool or executable composition, never Engine modules."
                "${_line}")
        endif()
    endforeach()
endfunction()
