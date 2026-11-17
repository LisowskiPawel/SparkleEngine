set(_shader_runtime_support)
foreach(_support IN LISTS SLANG_RUNTIME_SUPPORT_FILES)
    get_filename_component(_name "${_support}" NAME)
    list(APPEND _shader_runtime_support "${_name}")
endforeach()
foreach(_module IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    file(GLOB_RECURSE _module_files CONFIGURE_DEPENDS RELATIVE "${SLANG_SDK_ROOT}/Bin" "${_module}/*")
    if(NOT _module_files)
        message(FATAL_ERROR "Slang standard module is empty: ${_module}")
    endif()
    list(APPEND _shader_runtime_support ${_module_files})
endforeach()
set_property(TARGET ShaderCompiler APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES ${_shader_runtime_support})

foreach(slang_runtime_support_file IN LISTS SLANG_RUNTIME_SUPPORT_FILES)
    get_filename_component(slang_runtime_support_name "${slang_runtime_support_file}" NAME)
    add_custom_command(TARGET ShaderCompiler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${slang_runtime_support_file}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${slang_runtime_support_name}"
    )
endforeach()

foreach(slang_standard_module_directory IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    get_filename_component(slang_standard_module_name "${slang_standard_module_directory}" NAME)
    add_custom_command(TARGET ShaderCompiler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:ShaderCompiler>/${slang_standard_module_name}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${slang_standard_module_directory}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${slang_standard_module_name}"
    )
endforeach()
