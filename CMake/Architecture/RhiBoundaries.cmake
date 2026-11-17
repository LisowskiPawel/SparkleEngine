function(sparkle_check_rhi_boundaries _relative_path _lines)
    if(NOT _relative_path MATCHES "^Engine/RHI/")
        return()
    endif()

    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(_relative_path MATCHES "^Engine/RHI/" AND _line MATCHES "#include[^\n]*Renderer/|SparkleRenderer")
            sparkle_boundary_append_failure(
                "RHI_NO_RENDERER_DEPENDENCY"
                "${_relative_path}"
                "${_line_number}"
                "RHI must not include or link Renderer; the dependency direction is Renderer to public RHI."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/Public/" AND _line MATCHES "FrameGraph")
            sparkle_boundary_append_failure(
                "RHI_PUBLIC_NO_FRAME_GRAPH_POLICY"
                "${_relative_path}"
                "${_line_number}"
                "Frame-graph tracking and lifetime policy belongs to Renderer metadata, not public RHI contracts."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/" AND _line MATCHES "${SPARKLE_BOUNDARY_RENDERER_SHADER_DATA_REGEX}")
            sparkle_boundary_append_failure(
                "RHI_NO_RENDERER_SHADER_DATA"
                "${_relative_path}"
                "${_line_number}"
                "Renderer shader payload layouts belong in Renderer, not RHI resource contracts."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/Private/" AND
           NOT _relative_path MATCHES "^Engine/RHI/Private/(D3D12|Vulkan)/" AND
           _line MATCHES "${SPARKLE_BOUNDARY_NATIVE_API_REGEX}")
        	sparkle_boundary_append_failure(
        		"RHI_COMMON_NO_BACKEND_NATIVE"
        		"${_relative_path}"
        		"${_line_number}"
        		"Common RHI implementation must depend on neutral contracts; native API identifiers belong in the selected backend target."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/" AND _line MATCHES "Upscaler|RayReconstruction|GBuffer|RenderScene")
        	sparkle_boundary_append_failure(
        		"RHI_NO_RENDERER_FEATURE_POLICY"
        		"${_relative_path}"
        		"${_line_number}"
        		"Renderer feature policy must not appear in RHI contracts or implementation; use a neutral RHI capability or external-provider contract."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/Private/D3D12/" AND _line MATCHES "${SPARKLE_BOUNDARY_VULKAN_IN_D3D12_REGEX}")
            sparkle_boundary_append_failure(
                "D3D12_NO_VULKAN_BACKEND"
                "${_relative_path}"
                "${_line_number}"
                "D3D12 backend code must not include or use Vulkan backend/native identifiers."
                "${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/RHI/Private/Vulkan/" AND _line MATCHES "${SPARKLE_BOUNDARY_D3D12_IN_VULKAN_REGEX}")
            sparkle_boundary_append_failure(
                "VULKAN_NO_D3D12_BACKEND"
                "${_relative_path}"
                "${_line_number}"
                "Vulkan backend code must not include or use D3D12 backend/native identifiers."
                "${_line}")
        endif()
    endforeach()
endfunction()
