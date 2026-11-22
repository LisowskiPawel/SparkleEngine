# Engine modules share one linkage/export contract; each module owns its sources and dependencies.
function(sparkle_add_engine_module target_name export_definition)
    if(SPARKLE_BUILD_SHARED)
        add_library(${target_name} SHARED ${ARGN})
        target_compile_definitions(${target_name} PRIVATE ${export_definition})
        # The shipped DLL contains its private contributors even though its link API does not.
        set_property(TARGET ${target_name} APPEND PROPERTY TRANSITIVE_LINK_PROPERTIES SPARKLE_SOURCE_PACKAGES)
        set_property(TARGET ${target_name} PROPERTY INTERFACE_SPARKLE_SOURCE_PACKAGES
            "$<TARGET_PROPERTY:${target_name},SPARKLE_SOURCE_PACKAGES>")
    else()
        add_library(${target_name} STATIC ${ARGN})
        target_compile_definitions(${target_name} PUBLIC SPARKLE_STATIC)
    endif()
    sparkle_configure_native_target(${target_name})
    sparkle_configure_runtime_support_artifacts(${target_name})
    sparkle_stage_msvc_runtime(${target_name})
endfunction()
