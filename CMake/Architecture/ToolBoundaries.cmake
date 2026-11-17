function(sparkle_check_tool_boundaries _relative_path _lines)
    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(_line MATCHES "FormatToolWorkProgress|ParseToolWorkProgress" AND
           NOT _relative_path MATCHES "^Tools/Support/ToolSupport/(Private|Public)/ToolWorkProgress[.](cpp|h)$" AND
           NOT _relative_path STREQUAL "Tools/Support/ToolSupport/Private/ToolWorkProgressWriter.cpp" AND
           NOT _relative_path MATCHES "^Tools/Launcher/SparkleLauncher/Private/(Operations/LauncherOperationProgress|Gui/Operations/LauncherProgressStreamDecoder)[.]cpp$")
        	sparkle_boundary_append_failure(
        		"TOOL_PROGRESS_TRANSPORT_REMAINS_AT_PROCESS_BOUNDARY"
        		"${_relative_path}"
        		"${_line_number}"
        		"Tool producers publish semantic progress events; only shared progress support and Launcher process-boundary adapters may format or parse transport records."
        		"${_line}")
        endif()

        if(NOT _relative_path STREQUAL "Tools/Support/ToolSupport/Private/ToolWorkProgress.cpp" AND
           _line MATCHES "\\[PROGRESS\\]")
        	sparkle_boundary_append_failure(
        		"TOOL_PROGRESS_MARKER_HAS_ONE_OWNER"
        		"${_relative_path}"
        		"${_line_number}"
        		"The shared tool-progress implementation exclusively owns the transport marker."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Tools/Shaders/ShaderCompiler/Private/(Cooking|Verification)/" AND
           NOT _relative_path STREQUAL "Tools/Shaders/ShaderCompiler/Private/Cooking/GlobalShaderCooker.cpp" AND
           _line MATCHES "Cooking/GlobalShaderCooker\\.h")
            sparkle_boundary_append_failure(
                "SHADER_COOKING_NO_FACADE_DEPENDENCY"
                "${_relative_path}"
                "${_line_number}"
                "Shader cooking internals must depend on settings/result contracts, not upward on the cooker facade."
                "${_line}")
        endif()
    endforeach()
endfunction()
