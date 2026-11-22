# Stage the private batch tool through the same owner as the compiler payloads.
sparkle_configure_host_tool_target(ShaderMake)
add_custom_target(ShaderCompilerRuntime
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:ShaderCompiler>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ShaderMake>" "$<TARGET_FILE_DIR:ShaderCompiler>/ShaderMake.exe"
    DEPENDS ShaderMake
    VERBATIM)
set_target_properties(ShaderCompilerRuntime PROPERTIES FOLDER "Tools")
add_dependencies(ShaderCompiler ShaderCompilerRuntime)
get_target_property(_shadermake_source_package ShaderMake SPARKLE_SOURCE_PACKAGE)
set_property(TARGET ShaderCompiler PROPERTY SPARKLE_SOURCE_PACKAGE "${_shadermake_source_package}")

# The frontends and their SDK payload refresh together, even without a cooker relink.
set(_shader_runtime_files
    "${DXC_TOOL_EXECUTABLE}" "${DXC_RUNTIME_LIBRARY}"
    "${SLANG_TOOL_EXECUTABLE}" "${SLANG_RUNTIME_LIBRARY}"
    "${SPIRV_TOOLS_RUNTIME_LIBRARY}"
    ${SLANG_RUNTIME_SUPPORT_FILES})
set(_shader_runtime_support ShaderMake.exe)
foreach(_support IN LISTS _shader_runtime_files)
    get_filename_component(_name "${_support}" NAME)
    list(APPEND _shader_runtime_support "${_name}")
    add_custom_command(TARGET ShaderCompilerRuntime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_support}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_name}"
        VERBATIM
    )
endforeach()

foreach(_module IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    get_filename_component(_module_name "${_module}" NAME)
    file(GLOB_RECURSE _module_files CONFIGURE_DEPENDS RELATIVE "${SLANG_SDK_ROOT}/Bin" "${_module}/*")
    if(NOT _module_files)
        message(FATAL_ERROR "Slang standard module is empty: ${_module}")
    endif()
    list(APPEND _shader_runtime_support ${_module_files})
    add_custom_command(TARGET ShaderCompilerRuntime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_module_name}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_module}"
            "$<TARGET_FILE_DIR:ShaderCompiler>/${_module_name}"
        VERBATIM
    )
endforeach()
list(REMOVE_DUPLICATES _shader_runtime_support)
set_property(TARGET ShaderCompiler APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES ${_shader_runtime_support})
list(JOIN _shader_runtime_support "\",\n    \"" _shader_compiler_input_names)
configure_file("${CMAKE_CURRENT_LIST_DIR}/ShaderCompilerInputs.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ShaderCompilerInputs.h" @ONLY)
target_include_directories(ShaderCompiler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
