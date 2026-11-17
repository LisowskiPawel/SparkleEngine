# Both runtime and cook descriptors use the same source set and target requirements.
file(GLOB_RECURSE SPARKLE_RENDERER_SHADER_REGISTRATION_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*ShaderRegistration*.cpp
)
list(REMOVE_ITEM SPARKLE_RENDERER_PRIVATE_SOURCES ${SPARKLE_RENDERER_SHADER_REGISTRATION_SOURCES})

foreach(_registration IN ITEMS Shader ShaderCook)
    set(_object_target SparkleRenderer${_registration}Registrations)
    set(_interface_target SparkleRenderer${_registration}RegistrationObjects)
    add_library(${_object_target} OBJECT ${SPARKLE_RENDERER_SHADER_REGISTRATION_SOURCES})
    sparkle_enable_parallel_compilation(${_object_target})
    target_include_directories(${_object_target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Public/Diagnostics
    )
    target_compile_features(${_object_target} PUBLIC cxx_std_20)
    target_link_libraries(${_object_target} PUBLIC SparkleRHI PRIVATE SparkleCore)
    sparkle_target_precompile_headers(${_object_target} ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h)

    # Object files must reach the final executable so static registration is retained.
    add_library(${_interface_target} INTERFACE)
    target_sources(${_interface_target} INTERFACE $<TARGET_OBJECTS:${_object_target}>)
    target_link_libraries(${_interface_target} INTERFACE ${_object_target})
endforeach()
target_compile_definitions(SparkleRendererShaderCookRegistrations PRIVATE SPARKLE_SHADER_CONTRACTS_ONLY=1)
