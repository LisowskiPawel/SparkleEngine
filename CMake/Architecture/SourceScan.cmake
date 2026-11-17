function(sparkle_boundary_relative_path out_var absolute_path)
    file(RELATIVE_PATH _relative "${SPARKLE_REPO_ROOT}" "${absolute_path}")
    file(TO_CMAKE_PATH "${_relative}" _relative)
    set(${out_var} "${_relative}" PARENT_SCOPE)
endfunction()

function(sparkle_boundary_append_failure_text message_text)
    set_property(GLOBAL APPEND PROPERTY SPARKLE_BOUNDARY_FAILURES "${message_text}")
endfunction()

function(sparkle_boundary_append_failure rule relative_path line_number reason line_text)
    string(STRIP "${line_text}" _line_text)
    string(REPLACE ";" "," _line_text "${_line_text}")
    sparkle_boundary_append_failure_text("${relative_path}:${line_number}: [${rule}] ${reason} | ${_line_text}")
endfunction()

function(sparkle_boundary_collect_source_files out_var)
    set(_result "")
    foreach(_root IN LISTS ARGN)
        file(GLOB_RECURSE _files LIST_DIRECTORIES false "${SPARKLE_REPO_ROOT}/${_root}/*")
        foreach(_file IN LISTS _files)
            file(TO_CMAKE_PATH "${_file}" _normalized_file)
            if(_normalized_file MATCHES "${SPARKLE_BOUNDARY_SOURCE_FILE_REGEX}")
                list(APPEND _result "${_normalized_file}")
            endif()
        endforeach()
    endforeach()

    list(REMOVE_DUPLICATES _result)
    set(${out_var} ${_result} PARENT_SCOPE)
endfunction()

function(sparkle_boundary_scan_file absolute_path)
    sparkle_boundary_relative_path(_relative_path "${absolute_path}")
    # Policy definitions contain the identifiers that they reject in product source.
    if(_relative_path MATCHES "^CMake/(ArchitectureBoundaryCheck[.]cmake|Architecture/.*[.]cmake)$")
        return()
    endif()

    file(READ "${absolute_path}" _content)
    if(_content MATCHES "namespace[ \t\r\n]*\\{")
        sparkle_boundary_append_failure(
            "NO_ANONYMOUS_NAMESPACE"
            "${_relative_path}"
            "1"
            "Anonymous namespaces are forbidden; assign behavior to an owning type or an established named domain namespace."
            "namespace {")
    endif()
    string(REPLACE ";" "__SPARKLE_SEMICOLON__" _content "${_content}")
    string(REPLACE "\r\n" "\n" _content "${_content}")
    string(REPLACE "\r" "\n" _content "${_content}")
    string(REPLACE "\n" ";" _lines "${_content}")

    sparkle_check_module_boundaries("${_relative_path}" "${_lines}")
    sparkle_check_logging_boundaries("${_relative_path}" "${_lines}")
    sparkle_check_tool_boundaries("${_relative_path}" "${_lines}")
    sparkle_check_rhi_boundaries("${_relative_path}" "${_lines}")
    sparkle_check_renderer_contracts("${_relative_path}" "${_lines}")
    sparkle_check_renderer_composition("${_relative_path}" "${_lines}")
endfunction()
