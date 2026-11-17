function(sparkle_check_renderer_composition _relative_path _lines)
    if(NOT _relative_path MATCHES "^Engine/Renderer/")
        return()
    endif()

    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(_relative_path MATCHES "^Engine/Renderer/Private/Passes/.+Passes[.]cpp$")
            if(_line MATCHES "builder[.](AddPass|Dispatch|DispatchAsync)")
                sparkle_boundary_append_failure(
                    "RENDERER_PASS_ORCHESTRATION_NO_GPU_IMPLEMENTATION"
                    "${_relative_path}"
                    "${_line_number}"
                    "Passes files order named operations and do not define GPU dispatches directly."
                    "${_line}")
            endif()
            if((_line MATCHES "resources[.][A-Za-z0-9_.]+[ 	]*=" AND
                NOT _relative_path STREQUAL "Engine/Renderer/Private/Passes/Presentation/Upscaling/SceneUpscalingPasses.cpp") OR
               (_line MATCHES "[.]IsValid[(]" AND
                NOT _relative_path STREQUAL "Engine/Renderer/Private/Passes/Presentation/Upscaling/SceneUpscalingPasses.cpp") OR
               _line MATCHES "Get[A-Za-z0-9_]*(Upscaler|Reconstruction)Provider[(]")
                sparkle_boundary_append_failure(
                    "RENDERER_PASS_ORCHESTRATION_READS_AS_INTENT"
                    "${_relative_path}"
                    "${_line_number}"
                    "Passes orchestration calls named rendering operations; resource publication, handle guards, and provider selection stay in the called owner."
                    "${_line}")
            endif()
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Private/Frame/FramePipeline.h" AND
           _line MATCHES "#include[^\n]*Providers/RendererImageProviderStack[.]h")
            sparkle_boundary_append_failure(
                "RENDERER_FRAME_HEADER_NO_PROVIDER_IMPLEMENTATION"
                "${_relative_path}"
                "${_line_number}"
                "FramePipeline stores the focused provider graph key and an incomplete provider owner, not the provider implementation header."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/Frame/FramePipeline[.](cpp|h)$" AND
           _line MATCHES "(make_unique<RenderViewBuilder>|unique_ptr<RenderViewBuilder>|m_renderViewBuilder)")
            sparkle_boundary_append_failure(
                "RENDERER_STATELESS_VIEW_BUILD_HAS_NO_FAKE_LIFETIME"
                "${_relative_path}"
                "${_line_number}"
                "RenderView construction is stateless and remains a direct View operation rather than a heap-owned pipeline subsystem."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/Concurrency/Coordinator/RendererExecutionContext[.](cpp|h)$" AND
           _line MATCHES "#include[^\n]*\"(Diagnostics|Meshes|Pipeline|Providers|Scene|Textures|View)/")
            sparkle_boundary_append_failure(
                "RENDERER_EXECUTION_CONTEXT_REMAINS_ORCHESTRATION_ONLY"
                "${_relative_path}"
                "${_line_number}"
                "RendererExecutionContext may coordinate RendererHost and FramePipeline but must not reach into their subsystem implementations."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/Concurrency/Coordinator/RendererExecutionContext[.](cpp|h)$" AND
           _line MATCHES "(RenderFrameReadyCommand|RenderThreadCommandPayload|RenderThreadCommand[.]h)")
            sparkle_boundary_append_failure(
                "RENDERER_EXECUTION_CONTEXT_NO_THREAD_MAILBOX_ENVELOPE"
                "${_relative_path}"
                "${_line_number}"
                "RendererExecutionContext consumes executable renderer controls, not coordinator-only frame-ready mailbox commands."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/Host/RendererHost[.](cpp|h)$" AND
           _line MATCHES "#include[^\n]*\"(Meshes|Providers|Scene|Textures|View)/")
            sparkle_boundary_append_failure(
                "RENDERER_HOST_REMAINS_BACKEND_RUNTIME_OWNER"
                "${_relative_path}"
                "${_line_number}"
                "RendererHost owns backend/runtime composition; FramePipeline owns its Scene, View, cache, provider, capture, and feature systems."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Private/Host/RendererHost.h" AND
           _line MATCHES "Get(DeviceServices|RenderPassRuntimeCache|MemoryMonitor|GpuMeshCache|TextureCache|RenderScenePreparation|RenderViewBuilder|RenderViewPreparation|RenderViewState|RenderScene|ImageProviders|TaskExecutor)[ \t]*[(]")
            sparkle_boundary_append_failure(
                "RENDERER_HOST_NO_SERVICE_BAG_API"
                "${_relative_path}"
                "${_line_number}"
                "RendererHost must compose subsystems and expose semantic operations, not become a service-locator API."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Private/Frame/FramePipeline.h" AND
           _line MATCHES "(BuildRenderFrameGraph|PrepareFrameGraph|PrepareRenderingState|PublishFrameProducts)[ 	]*[(].*(PreparedRenderScene|RenderFrameIdentity|RenderFrameTime|RenderRayTracingFrameBindings|RenderView)")
            sparkle_boundary_append_failure(
                "RENDERER_FRAME_EXECUTION_ACCEPTS_CANONICAL_FRAME"
                "${_relative_path}"
                "${_line_number}"
                "Frame execution consumes the canonical RenderFrame instead of a parallel list of its components."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Public/Viewport/ViewportContracts.h" AND
           _line MATCHES "(RhiBackendApi|RayTracingExecutionFrontend|ViewportRenderProgressRoute)")
            sparkle_boundary_append_failure(
                "RENDERER_VIEWPORT_PROGRESS_NO_EXECUTION_DIAGNOSTICS"
                "${_relative_path}"
                "${_line_number}"
                "The public viewport product reports actionable work state, not private RHI backend or ray-tracing frontend diagnostics."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer/" AND
           NOT _relative_path MATCHES "ReferencePathTracer(Resources|Session)[.](cpp|h)$" AND
           _line MATCHES "ReferencePathTracerResources[ \t]*[&*]")
            sparkle_boundary_append_failure(
                "REFERENCE_PATH_TRACER_SESSION_OWNS_ACCUMULATOR_RESOURCES"
                "${_relative_path}"
                "${_line_number}"
                "Accumulator resources belong to the Reference session lifetime and must not be threaded through sibling feature functions."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/" AND
           NOT _relative_path MATCHES "^Engine/Renderer/Private/(Frame/Graph/RenderFrameGraphSettings[.]cpp|Passes/Presentation/PresentationPolicy[.]cpp|Passes/Scene/SceneRenderingPasses[.]cpp|Passes/Lighting/ReferencePathTracer/)" AND
           _line MATCHES "RenderViewMode::ReferencePathTracer")
            sparkle_boundary_append_failure(
                "REFERENCE_PATH_TRACER_SELECTOR_HOOK_BUDGET"
                "${_relative_path}"
                "${_line_number}"
                "The Reference selector is read only by frame-image topology policy, the alternate middle branch, and feature-local lifecycle state."
                "${_line}")
        endif()

        if(_relative_path MATCHES "${SPARKLE_BOUNDARY_RENDERER_HIGH_LEVEL_ORCHESTRATOR_REGEX}" AND
           _line MATCHES "${SPARKLE_BOUNDARY_RENDERER_PROVIDER_DETAIL_REGEX}")
            sparkle_boundary_append_failure(
                "RENDERER_ORCHESTRATOR_NO_PROVIDER_DETAILS"
                "${_relative_path}"
                "${_line_number}"
                "High-level renderer orchestrators must depend on provider interfaces/factories, not vendor implementations."
                "${_line}")
        endif()
    endforeach()
endfunction()
