# Runtime files and their distribution notices are staged together.
function(sparkle_stage_msvc_runtime target_name)
    get_target_property(_sparkle_target_type ${target_name} TYPE)
    if(MSVC AND _sparkle_target_type MATCHES "^(EXECUTABLE|SHARED_LIBRARY|MODULE_LIBRARY)$")
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
        set(CMAKE_INSTALL_DEBUG_LIBRARIES FALSE)
        set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY FALSE)
        set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
        set(CMAKE_INSTALL_MFC_LIBRARIES FALSE)
        set(CMAKE_INSTALL_OPENMP_LIBRARIES FALSE)
        include(InstallRequiredSystemLibraries)
        if(NOT CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
            message(FATAL_ERROR "MSVC redistributable runtime is unavailable for ${target_name}")
        endif()
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                "$<TARGET_FILE_DIR:${target_name}>"
            COMMAND_EXPAND_LISTS
            VERBATIM)
        set_property(TARGET ${target_name} PROPERTY
            SPARKLE_MSVC_RUNTIME_FILES "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}")
    endif()

endfunction()

function(sparkle_stage_d3d12_pix_event_runtime product_target)
    if(NOT TARGET Microsoft::WinPixEventRuntime OR NOT TARGET SparkleRHI_D3D12)
        add_custom_command(TARGET ${product_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -f
                "$<TARGET_FILE_DIR:${product_target}>/WinPixEventRuntime.dll"
                "$<TARGET_FILE_DIR:${product_target}>/WinPixEventRuntime-LICENSE.txt"
            VERBATIM
        )
        return()
    endif()

    get_target_property(_sparkle_pix_enabled SparkleRHI_D3D12 SPARKLE_PIX_EVENTS_ENABLED)
    get_target_property(_sparkle_pix_license Microsoft::WinPixEventRuntime SPARKLE_LICENSE_FILE)
    add_custom_command(TARGET ${product_target} POST_BUILD
        COMMAND "$<${_sparkle_pix_enabled}:${CMAKE_COMMAND}>"
            "$<${_sparkle_pix_enabled}:-E>"
            "$<${_sparkle_pix_enabled}:copy_if_different>"
            "$<${_sparkle_pix_enabled}:$<TARGET_FILE:Microsoft::WinPixEventRuntime>>"
            "$<${_sparkle_pix_enabled}:$<TARGET_FILE_DIR:${product_target}>>"
        COMMAND "$<${_sparkle_pix_enabled}:${CMAKE_COMMAND}>"
            "$<${_sparkle_pix_enabled}:-E>"
            "$<${_sparkle_pix_enabled}:copy_if_different>"
            "$<${_sparkle_pix_enabled}:${_sparkle_pix_license}>"
            "$<${_sparkle_pix_enabled}:$<TARGET_FILE_DIR:${product_target}>/WinPixEventRuntime-LICENSE.txt>"
        COMMAND "$<$<NOT:${_sparkle_pix_enabled}>:${CMAKE_COMMAND}>"
            "$<$<NOT:${_sparkle_pix_enabled}>:-E>"
            "$<$<NOT:${_sparkle_pix_enabled}>:rm>"
            "$<$<NOT:${_sparkle_pix_enabled}>:-f>"
            "$<$<NOT:${_sparkle_pix_enabled}>:$<TARGET_FILE_DIR:${product_target}>/WinPixEventRuntime.dll>"
            "$<$<NOT:${_sparkle_pix_enabled}>:$<TARGET_FILE_DIR:${product_target}>/WinPixEventRuntime-LICENSE.txt>"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endfunction()

function(sparkle_declare_runtime_dll_owner product_target)
    if(NOT TARGET ${product_target})
        message(FATAL_ERROR "Unknown Sparkle product target '${product_target}'")
    endif()

    if(NOT SPARKLE_BUILD_SHARED)
        return()
    endif()

    foreach(runtime_target IN LISTS ARGN)
        if(TARGET ${runtime_target})
            set_property(TARGET ${product_target} APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES
                "$<TARGET_FILE_NAME:${runtime_target}>")
            add_custom_command(TARGET ${product_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${runtime_target}>"
                    "$<TARGET_FILE_DIR:${product_target}>"
                COMMENT "Copying ${runtime_target} runtime DLL for ${product_target}"
                VERBATIM
            )
        endif()
    endforeach()
endfunction()

function(sparkle_stage_nvidia_streamline_runtime product_target)
    if(NOT TARGET ${product_target})
        message(FATAL_ERROR "Unknown Sparkle product target '${product_target}'")
    endif()

    if(NOT SPARKLE_ENABLE_NVIDIA_STREAMLINE)
        return()
    endif()

    if(NOT DEFINED SPARKLE_NVIDIA_STREAMLINE_RUNTIME_DLLS)
        message(FATAL_ERROR
            "NVIDIA Streamline runtime DLL list is not configured. "
            "FetchDependencies.cmake must run before staging Streamline.")
    endif()

    add_custom_target(${product_target}NvidiaNotices)
    add_dependencies(${product_target} ${product_target}NvidiaNotices)

    # The SDK's top-level MIT notice does not cover every runtime plugin.
    foreach(_notice IN ITEMS license.txt 3rd-party-licenses.md
            bin/x64/nis.license.txt bin/x64/nvngx_dlss.license.txt bin/x64/reflex.license.txt)
        set(_notice_source "${SPARKLE_NVIDIA_STREAMLINE_ROOT}/${_notice}")
        if(NOT EXISTS "${_notice_source}")
            message(FATAL_ERROR "NVIDIA Streamline runtime notice is missing: '${_notice_source}'")
        endif()
        get_filename_component(_notice_name "${_notice}" NAME)
        add_custom_command(TARGET ${product_target}NvidiaNotices POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${product_target}>/Notices/NVIDIA"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_notice_source}" "$<TARGET_FILE_DIR:${product_target}>/Notices/NVIDIA/${_notice_name}"
            COMMENT "Copying NVIDIA runtime notice ${_notice_name} for ${product_target}"
            VERBATIM
        )
    endforeach()

    add_custom_command(TARGET ${product_target}NvidiaNotices POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/NVIDIA/README.md"
            "$<TARGET_FILE_DIR:${product_target}>/Notices/NVIDIA/README.md"
        COMMENT "Copying NVIDIA component distribution terms for ${product_target}"
        VERBATIM
    )

    foreach(runtime_dll IN LISTS SPARKLE_NVIDIA_STREAMLINE_RUNTIME_DLLS)
        if(NOT EXISTS "${runtime_dll}")
            message(FATAL_ERROR "NVIDIA Streamline runtime DLL is missing: '${runtime_dll}'")
        endif()
        add_custom_command(TARGET ${product_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${runtime_dll}"
                "$<TARGET_FILE_DIR:${product_target}>"
            COMMENT "Copying NVIDIA Streamline runtime ${runtime_dll} for ${product_target}"
            VERBATIM
        )
    endforeach()
endfunction()

function(sparkle_stage_external_capture_notices product_target)
    if(NOT TARGET SparkleRHI)
        return()
    endif()
    get_target_property(_capture_notices SparkleRHI SPARKLE_EXTERNAL_CAPTURE_NOTICES)
    if(NOT _capture_notices)
        return()
    endif()
    set(_capture_profile "$<AND:$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>,$<OR:$<CONFIG:DebugEditor>,$<CONFIG:DevelopmentEditor>>>")
    foreach(_notice IN LISTS _capture_notices)
        string(REPLACE "|" ";" _notice_parts "${_notice}")
        list(GET _notice_parts 0 _notice_source)
        list(GET _notice_parts 1 _notice_name)
        add_custom_command(TARGET ${product_target} POST_BUILD
            COMMAND "$<${_capture_profile}:${CMAKE_COMMAND}>" "$<${_capture_profile}:-E>" "$<${_capture_profile}:copy_if_different>"
                "$<${_capture_profile}:${_notice_source}>" "$<${_capture_profile}:$<TARGET_FILE_DIR:${product_target}>/${_notice_name}>"
            COMMAND "$<$<NOT:${_capture_profile}>:${CMAKE_COMMAND}>" "$<$<NOT:${_capture_profile}>:-E>" "$<$<NOT:${_capture_profile}>:rm>" "$<$<NOT:${_capture_profile}>:-f>"
                "$<$<NOT:${_capture_profile}>:$<TARGET_FILE_DIR:${product_target}>/${_notice_name}>"
            COMMAND_EXPAND_LISTS VERBATIM)
    endforeach()
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/../Diagnostics/GpuCrashArtifacts.cmake")
