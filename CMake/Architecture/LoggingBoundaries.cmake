function(sparkle_check_logging_boundaries _relative_path _lines)
    set(_line_number 0)
    foreach(_line IN LISTS _lines)
        math(EXPR _line_number "${_line_number} + 1")
        string(REPLACE "__SPARKLE_SEMICOLON__" ";" _line "${_line}")

        if(NOT _relative_path STREQUAL "Engine/Core/Private/Diagnostics/Logger.cpp" AND
           _line MATCHES "GetOrCreateLogger")
        	sparkle_boundary_append_failure(
        		"LOGGING_REGISTRY_REMAINS_CORE_PRIVATE"
        		"${_relative_path}"
        		"${_line_number}"
        		"Consumers declare a LogCategory; logger registry creation and lifetime remain private to Core diagnostics."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Core/Private/Diagnostics/(Logger|LoggerBootstrap)[.](cpp|h)$" AND
           _line MATCHES "FileSystemPathState|Core/Public/Paths/(FileSystemPaths|CookedAssetPaths|ProductUserStatePaths)|GetProductUserStatePaths")
        	sparkle_boundary_append_failure(
        		"LOGGING_BOOTSTRAP_USES_STATELESS_PATH_POLICY"
        		"${_relative_path}"
        		"${_line_number}"
        		"Logger bootstrap depends only on the private stateless log-path policy; aggregate path state can emit diagnostics while it initializes."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Core/Private/Diagnostics/LoggerBootstrap[.](cpp|h)$" AND
           _line MATCHES "Diagnostics/Logger[.]h|Get(CoreLogger|Logger|ActiveLogFilePath)[(]|Logging::Initialize")
        	sparkle_boundary_append_failure(
        		"LOGGING_BOOTSTRAP_REMAINS_REGISTRY_INDEPENDENT"
        		"${_relative_path}"
        		"${_line_number}"
        		"Sink and initial-level setup must not re-enter the logger registry while initialization holds its lock."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Core/Private/Paths/LogPathPolicy[.](cpp|h)$" AND
           _line MATCHES "Diagnostics/|Logger[.]h|FileSystemPathState|GetProductUserStatePaths|Core/Public/Paths/FileSystemPaths")
        	sparkle_boundary_append_failure(
        		"LOG_PATH_POLICY_REMAINS_BOOTSTRAP_SAFE"
        		"${_relative_path}"
        		"${_line_number}"
        		"The log-path policy may use only stateless user-state and filesystem discovery and must not call logging or aggregate path state."
        		"${_line}")
        endif()

        if(_relative_path MATCHES "^Engine/Core/Private/Paths/(FileSystemDiscovery|UserStatePaths)[.](cpp|h)$" AND
           _line MATCHES "Diagnostics/|Logger[.]h|Get(FileSystemPathState|ProductUserStatePaths)")
        	sparkle_boundary_append_failure(
        		"BOOTSTRAP_PATH_DISCOVERY_REMAINS_STATELESS"
        		"${_relative_path}"
        		"${_line_number}"
        		"Bootstrap filesystem and user-state discovery must not call logging or aggregate path state."
        		"${_line}")
        endif()
    endforeach()
endfunction()
