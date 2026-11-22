# Stage the private batch tool through the same owner as the compiler payloads.
sparkle_configure_host_tool_target(ShaderMake UPSTREAM)
add_custom_target(ShaderCompilerRuntime
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:ShaderCompiler>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ShaderMake>" "$<TARGET_FILE_DIR:ShaderCompiler>/ShaderMake.exe"
    DEPENDS ShaderMake
    VERBATIM)
set_target_properties(ShaderCompilerRuntime PROPERTIES FOLDER "Tools")
add_dependencies(ShaderCompiler ShaderCompilerRuntime)
sparkle_target_source_package_dependencies(ShaderCompiler ShaderMake)

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

set(_shader_module_names)
set(_shader_module_files)
foreach(_module IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    get_filename_component(_module_name "${_module}" NAME)
    file(GLOB_RECURSE _module_files CONFIGURE_DEPENDS RELATIVE "${SLANG_SDK_ROOT}/Bin" "${_module}/*")
    if(NOT _module_files)
        message(FATAL_ERROR "Slang standard module is empty: ${_module}")
    endif()
    list(APPEND _shader_module_names "${_module_name}")
    list(APPEND _shader_module_files ${_module_files})
    list(APPEND _shader_runtime_support ${_module_files})
endforeach()
configure_file("${CMAKE_CURRENT_LIST_DIR}/StageSlangStandardModules.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/StageSlangStandardModules.cmake" @ONLY)
add_custom_command(TARGET ShaderCompilerRuntime POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        "-DSPARKLE_SLANG_MODULE_SOURCE_DIRECTORY=${SLANG_SDK_ROOT}/Bin"
        "-DSPARKLE_RUNTIME_DIRECTORY=$<TARGET_FILE_DIR:ShaderCompiler>"
        -P "${CMAKE_CURRENT_BINARY_DIR}/generated/StageSlangStandardModules.cmake"
    VERBATIM)
list(REMOVE_DUPLICATES _shader_runtime_support)
set_property(TARGET ShaderCompiler APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES ${_shader_runtime_support})
list(JOIN _shader_runtime_support "\",\n    \"" _shader_compiler_input_names)
configure_file("${CMAKE_CURRENT_LIST_DIR}/ShaderCompilerInputs.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ShaderCompilerInputs.h" @ONLY)
target_include_directories(ShaderCompiler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
