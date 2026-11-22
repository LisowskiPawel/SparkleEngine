file(GLOB_RECURSE SHADER_COMPILER_PRIVATE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

file(GLOB_RECURSE SHADER_COMPILER_PRIVATE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
)

if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    list(FILTER SHADER_COMPILER_PRIVATE_SOURCES EXCLUDE REGEX "/Cooking/Publication/ShaderDebugSourceIdentity\\.cpp$")
endif()

set(SHADER_COMPILER_CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/ShaderCompilerCli.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/ShaderCookOptions.cpp)
list(REMOVE_ITEM SHADER_COMPILER_PRIVATE_SOURCES ${SHADER_COMPILER_CLI_SOURCES})

add_executable(ShaderCompiler
    ${SHADER_COMPILER_PRIVATE_SOURCES}
    ${SHADER_COMPILER_PRIVATE_HEADERS}
)
sparkle_configure_host_tool_target(ShaderCompiler)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ShaderCompilerSdks.cmake")

target_include_directories(ShaderCompiler
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_SOURCE_DIR}/Engine
        ${CMAKE_SOURCE_DIR}/Engine/Core/Public/Diagnostics
)

target_compile_features(ShaderCompiler PRIVATE cxx_std_20)

add_library(ShaderCompilerCli OBJECT ${SHADER_COMPILER_CLI_SOURCES})
sparkle_configure_host_tool_target(ShaderCompilerCli)
target_include_directories(ShaderCompilerCli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Private
    ${CMAKE_SOURCE_DIR}/Engine
    ${CMAKE_SOURCE_DIR}/Engine/Core/Public/Diagnostics)
target_link_libraries(ShaderCompilerCli PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json SparkleCore SparkleRHI ToolSupport)
target_compile_features(ShaderCompilerCli PRIVATE cxx_std_20)
sparkle_target_precompile_headers(ShaderCompilerCli <CLI/CLI.hpp>)
target_link_libraries(ShaderCompiler PRIVATE ShaderCompilerCli)

if(MSVC)
    # Argument parsing is cold code; bound header-only CLI11 inlining without changing shader compilation.
    target_compile_options(ShaderCompilerCli PRIVATE
            "$<$<CONFIG:DevelopmentEditor,DevelopmentGame,ShippingEditor,ShippingGame>:/O1>"
            "$<$<CONFIG:DevelopmentEditor,DevelopmentGame,ShippingEditor,ShippingGame>:/Ob0>")
endif()

sparkle_target_precompile_headers(ShaderCompiler
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

target_link_libraries(ShaderCompiler
    PRIVATE
        nlohmann_json::nlohmann_json
        ToolSupport
        SparkleCore
        SparkleRHI
        SparkleRendererShaderCookRegistrationObjects
        Dxc::Dxc
        spirv_reflect
        SPIRVTools::SPIRVTools
)

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/ShaderCompilerRuntime.cmake")

sparkle_configure_development_tool_artifacts(ShaderCompiler)
sparkle_stage_msvc_runtime(ShaderCompiler)
sparkle_stage_linked_runtime(ShaderCompiler)
sparkle_get_project_cooked_directory(SHADER_COMPILER_VALIDATION_COOKED_DIRECTORY Shared)

add_custom_target(ShaderCompilerCliValidation
    COMMAND ${CMAKE_COMMAND}
        "-DSHADER_COMPILER_EXECUTABLE=$<TARGET_FILE:ShaderCompiler>"
        "-DWORKING_DIRECTORY=${CMAKE_SOURCE_DIR}"
        "-DREPRESENTATIVE_PROJECT=Shared"
        "-DCOOKED_SHADER_DIRECTORY=${SHADER_COMPILER_VALIDATION_COOKED_DIRECTORY}/Shaders"
        "-DREPRESENTATIVE_SHADER=ComputeClearCS"
        "-DREPRESENTATIVE_VIRTUAL_SOURCE=/Engine/Passes/Compute/ComputeClear.hlsl"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/ValidateShaderCompilerCli.cmake"
    DEPENDS ShaderCompiler
    USES_TERMINAL
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${SHADER_COMPILER_PRIVATE_SOURCES} ${SHADER_COMPILER_PRIVATE_HEADERS} ${SHADER_COMPILER_CLI_SOURCES})

sparkle_configure_gpu_crash_artifacts(ShaderCompiler)
