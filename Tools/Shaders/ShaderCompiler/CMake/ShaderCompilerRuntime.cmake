# Stage the private batch tool through the same owner as the compiler payloads.
sparkle_configure_host_tool_target(ShaderMake)
add_custom_target(ShaderCompilerBatchTool
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:ShaderCompiler>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ShaderMake>" "$<TARGET_FILE_DIR:ShaderCompiler>/ShaderMake.exe"
    DEPENDS ShaderMake
    VERBATIM)
set_target_properties(ShaderCompilerBatchTool PROPERTIES FOLDER "Tools")
add_dependencies(ShaderCompiler ShaderCompilerBatchTool)
set_property(TARGET ShaderCompiler APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES ShaderMake.exe)
get_target_property(_shadermake_source_package ShaderMake SPARKLE_SOURCE_PACKAGE)
set_property(TARGET ShaderCompiler PROPERTY SPARKLE_SOURCE_PACKAGE "${_shadermake_source_package}")

# Declare each Slang payload's staging and release inventory together.
set(_shader_runtime_support)
foreach(_support IN LISTS SLANG_RUNTIME_SUPPORT_FILES)
    get_filename_component(_name "${_support}" NAME)
    list(APPEND _shader_runtime_support "${_name}")
    add_custom_command(TARGET ShaderCompiler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_support}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_name}"
    )
endforeach()

foreach(_module IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    get_filename_component(_module_name "${_module}" NAME)
    file(GLOB_RECURSE _module_files CONFIGURE_DEPENDS RELATIVE "${SLANG_SDK_ROOT}/Bin" "${_module}/*")
    if(NOT _module_files)
        message(FATAL_ERROR "Slang standard module is empty: ${_module}")
    endif()
    list(APPEND _shader_runtime_support ${_module_files})
    add_custom_command(TARGET ShaderCompiler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_module_name}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_module}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_module_name}"
    )
endforeach()
set_property(TARGET ShaderCompiler APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES ${_shader_runtime_support})
