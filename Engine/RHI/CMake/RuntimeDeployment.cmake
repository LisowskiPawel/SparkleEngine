# Product deployment for RHI-owned PIX events and native capture notices.
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
    set(_runtime_directory "$<TARGET_FILE_DIR:${product_target}>")
    add_custom_command(TARGET ${product_target} POST_BUILD
        COMMAND "$<${_sparkle_pix_enabled}:${CMAKE_COMMAND};-E;copy_if_different;$<TARGET_FILE:Microsoft::WinPixEventRuntime>;${_runtime_directory}>"
        COMMAND "$<${_sparkle_pix_enabled}:${CMAKE_COMMAND};-E;copy_if_different;${_sparkle_pix_license};${_runtime_directory}/WinPixEventRuntime-LICENSE.txt>"
        COMMAND "$<$<NOT:${_sparkle_pix_enabled}>:${CMAKE_COMMAND};-E;rm;-f;${_runtime_directory}/WinPixEventRuntime.dll;${_runtime_directory}/WinPixEventRuntime-LICENSE.txt>"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endfunction()

function(sparkle_stage_external_capture_notices product_target)
    if(NOT TARGET SparkleRHI)
        return()
    endif()
    get_target_property(_capture_notices SparkleRHI SPARKLE_EXTERNAL_CAPTURE_NOTICES)
    if(NOT _capture_notices)
        return()
    endif()
    set(_capture_profile "$<AND:$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>,$<CONFIG:DebugEditor,DevelopmentEditor>>")
    set(_runtime_directory "$<TARGET_FILE_DIR:${product_target}>")
    foreach(_notice IN LISTS _capture_notices)
        string(REPLACE "|" ";" _notice_parts "${_notice}")
        list(GET _notice_parts 0 _notice_source)
        list(GET _notice_parts 1 _notice_name)
        add_custom_command(TARGET ${product_target} POST_BUILD
            COMMAND "$<${_capture_profile}:${CMAKE_COMMAND};-E;copy_if_different;${_notice_source};${_runtime_directory}/${_notice_name}>"
            COMMAND "$<$<NOT:${_capture_profile}>:${CMAKE_COMMAND};-E;rm;-f;${_runtime_directory}/${_notice_name}>"
            COMMAND_EXPAND_LISTS VERBATIM)
    endforeach()
endfunction()
