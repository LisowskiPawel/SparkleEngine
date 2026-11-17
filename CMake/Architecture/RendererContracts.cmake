function(sparkle_check_renderer_contracts _relative_path _lines)
    if(NOT _relative_path MATCHES "^Engine/Renderer/")
        return()
    endif()

    if(_relative_path MATCHES "^Engine/Renderer/(Public|Private)/Editor/")
        sparkle_boundary_append_failure(
            "RENDERER_NO_EDITOR_SUBSYSTEM"
            "${_relative_path}"
            "1"
            "Renderer may expose rendering and UI-rendering contracts, but Editor-owned state and naming belong to the Editor or Application modules."
            "${_relative_path}")
    endif()

    if(_relative_path MATCHES "^Engine/Renderer/Private/Passes/(Debug/Debug|GBuffer/(GBuffer|RasterizedGBuffer|RayTracingGBuffer|SceneDepth|SkyMotionVectors)|Lighting/Lighting|Lighting/Direct/DirectLightReservoir|Lighting/ReferencePathTracer/ReferencePathTracer|Lighting/Restir/Restir(Direct|Indirect)?Lighting|Lighting/Restir/RestirIndirectReservoirs|Lighting/Shadows/ShadowVisibility|PostProcessing/(Exposure|ExposureMetering|ExposureMomentChain|ExposureMomentPasses|PostProcessing)|Presentation/(LinearUpscaling|Presentation|SceneUpscaling|Upscaling)|RayTracing/RayTracingScene)[.](h|cpp)$")
        sparkle_boundary_append_failure(
            "RENDERER_PASS_FILE_ROLE_NAMING"
            "${_relative_path}"
            "1"
            "Pass orchestration files use the Passes suffix; singular or PassDefinitions files define GPU work; target/resource files name that responsibility explicitly."
            "${_relative_path}")
    endif()

    if(_relative_path MATCHES "^Engine/Renderer/Private/Passes/Lighting/RealTimeLightingPasses[.](h|cpp)$")
        sparkle_boundary_append_failure(
            "RENDERER_REALTIME_PATH_TRACER_OWNS_LIGHTING"
            "${_relative_path}"
            "1"
            "The real-time path-tracer composition directly owns its GBuffer, lighting, reconstruction, and publication sequence; do not restore a forwarding lighting wrapper."
            "${_relative_path}")
    endif()

    if(_relative_path MATCHES "${SPARKLE_BOUNDARY_RENDERER_PRIVATE_ONLY_PUBLIC_PATH_REGEX}")
        sparkle_boundary_append_failure(
            "RENDERER_IMPLEMENTATION_CONTRACT_REMAINS_PRIVATE"
            "${_relative_path}"
            "1"
            "Frame-graph, shader-binding, CVar, prepared-scene, GPU-mesh, and default-resource implementation contracts must remain under Renderer/Private."
            "${_relative_path}")
    endif()

    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(_relative_path STREQUAL "Engine/Renderer/Private/Passes/Presentation/Upscaling/SceneUpscalingPasses.cpp" AND
           _line MATCHES "AddUpscalerPass|Get[A-Za-z0-9_]*UpscalerProvider|IUpscalerProvider")
            sparkle_boundary_append_failure(
                "RENDERER_UPSCALING_SELECTOR_NAMES_IMPLEMENTATIONS"
                "${_relative_path}"
                "${_line_number}"
                "The scene-upscaling selector names and invokes each supported implementation; generic provider evaluation stays below the selected operation."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/FrameGraph/" AND
           _line MATCHES "RenderViewMode|ViewportRenderRequest|GetViewMode|#include[^\n]*Viewport/(ViewportContracts|RenderViewMode)[.]h")
            sparkle_boundary_append_failure(
                "RENDERER_FRAME_GRAPH_EXCLUDES_VIEW_SELECTION"
                "${_relative_path}"
                "${_line_number}"
                "Frame-graph infrastructure owns graph authoring and execution; view selection and viewport requests stay in high-level composition."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Private/(FrameGraph|Pipeline)/" AND
           _line MATCHES "#include[^\n]*Scene/|RayTracingShaderTablePlan|RayTracingShaderTableHitGroup|ResolveSceneRecordGroups|RecordMaterialization")
            sparkle_boundary_append_failure(
                "RENDERER_GRAPH_RUNTIME_EXCLUDES_SCENE_POLICY"
                "${_relative_path}"
                "${_line_number}"
                "Graph and pipeline infrastructure consume resolved shader-table records; scene/material mapping and scene metrics belong to the ray-material owner."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Private/Passes/Scene/SceneRenderingPasses.cpp" AND
           _line MATCHES "#include[^\n]*Passes/(GBuffer|Lighting/RealTimeLighting|Lighting/Restir|Lighting/ReferencePathTracer/(ReferencePathTracerDisplay|ReferencePathTracerResources|ReferencePathTracerSession|ReferencePathTracerTransport))" AND
           NOT _line MATCHES "#include[^\n]*Passes/Lighting/(Restir/Reconstruction/RestirRayReconstruction|ReferencePathTracer/ReferencePathTracerSession)[.]h")
            sparkle_boundary_append_failure(
                "RENDERER_SCENE_RENDERING_COMPOSITION_READS_AS_INTENT"
                "${_relative_path}"
                "${_line_number}"
                "Scene-rendering composition selects the renderer, invokes reconstruction, and publishes session products through narrow APIs; each renderer's GBuffer, ReSTIR transport, resource, and pass mechanics stay behind its AddPasses entry."
                "${_line}")
        endif()

                if(_relative_path MATCHES "^Engine/Renderer/" AND _line MATCHES "${SPARKLE_BOUNDARY_NATIVE_API_REGEX}")
        			sparkle_boundary_append_failure(
        				"RENDERER_NO_BACKEND_NATIVE"
        				"${_relative_path}"
        				"${_line_number}"
        				"Renderer code must not depend on D3D12/Vulkan native APIs."
        				"${_line}")
                endif()

        if(_relative_path MATCHES "^Engine/Renderer/" AND _line MATCHES "RHI/Private")
            sparkle_boundary_append_failure(
                "RENDERER_NO_RHI_PRIVATE"
                "${_relative_path}"
                "${_line_number}"
                "Renderer may consume only public RHI contracts, never backend or common RHI implementation headers."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Public/" AND
           _line MATCHES "#include[^\n]*(Renderer/Private/|\.\./Private/)")
            sparkle_boundary_append_failure(
                "RENDERER_PUBLIC_NO_PRIVATE_DEPENDENCY"
                "${_relative_path}"
                "${_line_number}"
                "Renderer public contracts must not expose or depend on Renderer implementation headers."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/" AND
           _line MATCHES "EditorTexture|EngineRenderingSettings(Section|Persistence)")
            sparkle_boundary_append_failure(
                "RENDERER_NO_EDITOR_OR_PERSISTENCE_POLICY"
                "${_relative_path}"
                "${_line_number}"
                "Renderer owns rendering state and GPU execution, not Editor interaction models or filesystem persistence policy."
                "${_line}")
        endif()

        if(_relative_path STREQUAL "Engine/Renderer/Public/Viewport/ViewportContracts.h" AND
           _line MATCHES "UiTextureHandle|EditorTextureHandle")
            sparkle_boundary_append_failure(
                "RENDER_PRODUCT_NO_UI_BINDING"
                "${_relative_path}"
                "${_line_number}"
                "Viewport render products contain render semantics only; UI presentation bindings cross the dedicated Renderer UI seam."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/" AND
           NOT _relative_path MATCHES "${SPARKLE_BOUNDARY_RENDERER_VENDOR_INTEROP_REGEX}" AND
           _line MATCHES "ERhiBackendApi::(D3D12|Vulkan)")
            sparkle_boundary_append_failure(
                "RENDERER_NO_BACKEND_POLICY_BRANCH"
                "${_relative_path}"
                "${_line_number}"
                "Renderer policy must branch on neutral capabilities; backend identity is reserved for dedicated external-provider interop adapters."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/Public/" AND _line MATCHES "RenderHardwareInterface|RenderDeviceServices")
            sparkle_boundary_append_failure(
                "RENDERER_PUBLIC_NO_RHI_DEVICE_ESCAPE_HATCH"
                "${_relative_path}"
                "${_line_number}"
                "Renderer public APIs must expose focused renderer operations instead of the complete RHI device facade."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Renderer/" AND _line MATCHES "${SPARKLE_BOUNDARY_NATIVE_PTLAS_REGEX}")
            sparkle_boundary_append_failure(
                "RENDERER_NO_NATIVE_PTLAS"
                "${_relative_path}"
                "${_line_number}"
                "Renderer code must use backend-neutral RHI PTLAS structs; native Vulkan/D3D12/NVAPI PTLAS identifiers belong in backend-private RHI code."
                "${_line}")
        endif()
    endforeach()
endfunction()
