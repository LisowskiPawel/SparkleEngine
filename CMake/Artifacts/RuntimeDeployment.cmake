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

function(sparkle_stage_linked_runtime product_target)
    set_property(TARGET ${product_target} APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES
        "$<PATH:GET_FILENAME,$<TARGET_RUNTIME_DLLS:${product_target}>>")
    add_custom_command(TARGET ${product_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            "-DSPARKLE_RUNTIME_DLLS=$<TARGET_RUNTIME_DLLS:${product_target}>"
            "-DSPARKLE_RUNTIME_DIRECTORY=$<TARGET_FILE_DIR:${product_target}>"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/StageRuntimeDlls.cmake"
        COMMENT "Staging linked runtime DLLs for ${product_target}"
        VERBATIM)
endfunction()
