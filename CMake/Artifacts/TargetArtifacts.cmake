function(_sparkle_set_target_artifact_directories target_name runtime_root symbol_owner)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Unknown Sparkle target '${target_name}'")
    endif()

    sparkle_stage_msvc_runtime(${target_name})

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${runtime_root}/$<CONFIG>"
        LIBRARY_OUTPUT_DIRECTORY "${runtime_root}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${_sparkle_library_artifact_root}/${symbol_owner}/$<CONFIG>"
        PDB_OUTPUT_DIRECTORY "${_sparkle_symbol_artifact_root}/${symbol_owner}/$<CONFIG>"
        COMPILE_PDB_OUTPUT_DIRECTORY "${_sparkle_symbol_artifact_root}/${symbol_owner}/$<CONFIG>/obj"
    )

    set(_sparkle_output_configurations ${CMAKE_CONFIGURATION_TYPES})
    if(NOT _sparkle_output_configurations AND CMAKE_BUILD_TYPE)
        set(_sparkle_output_configurations "${CMAKE_BUILD_TYPE}")
    endif()
    foreach(config_type IN LISTS _sparkle_output_configurations)
        string(TOUPPER "${config_type}" config_upper)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${runtime_root}/${config_type}"
            LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${runtime_root}/${config_type}"
            ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${_sparkle_library_artifact_root}/${symbol_owner}/${config_type}"
            PDB_OUTPUT_DIRECTORY_${config_upper} "${_sparkle_symbol_artifact_root}/${symbol_owner}/${config_type}"
            COMPILE_PDB_OUTPUT_DIRECTORY_${config_upper} "${_sparkle_symbol_artifact_root}/${symbol_owner}/${config_type}/obj"
        )
    endforeach()
endfunction()

function(sparkle_configure_launcher_artifacts target_name)
    _sparkle_set_target_artifact_directories(${target_name} "${_sparkle_tool_artifact_root}/SparkleLauncher/Binaries" "launcher")
endfunction()

function(sparkle_configure_development_tool_artifacts target_name)
    _sparkle_set_target_artifact_directories(${target_name} "${_sparkle_tool_artifact_root}/${target_name}/Binaries" "tools/${target_name}")
endfunction()

function(sparkle_configure_runtime_support_artifacts target_name)
    _sparkle_set_target_artifact_directories(
        ${target_name}
        "${_sparkle_runtime_support_artifact_root}/${target_name}"
        "runtime-support/${target_name}")
endfunction()

function(sparkle_configure_project_artifacts target_name project_name product_role)
    _sparkle_set_target_artifact_directories(
        ${target_name}
        "${_sparkle_project_artifact_root}/${project_name}/Binaries"
        "projects/${project_name}/${product_role}")
    sparkle_stage_d3d12_pix_event_runtime(${target_name})
    sparkle_stage_external_capture_notices(${target_name})
endfunction()

function(sparkle_get_project_cooked_directory output_variable project_name)
    set(${output_variable} "${_sparkle_project_artifact_root}/${project_name}/Cooked" PARENT_SCOPE)
endfunction()
