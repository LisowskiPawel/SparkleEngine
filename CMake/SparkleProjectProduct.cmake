# Shared configuration for runnable Sparkle project products.

set(SPARKLE_WINDOWS_APPLICATION_MANIFEST
    "${SPARKLE_REPOSITORY_ROOT}/Engine/Platform/Private/Window/SparkleApplication.manifest")

function(sparkle_configure_project_product target_name project_name product_role)
    sparkle_configure_project_artifacts("${target_name}" "${project_name}" "${product_role}")
    sparkle_stage_msvc_runtime(${target_name})
    sparkle_stage_d3d12_pix_event_runtime(${target_name})
    sparkle_stage_external_capture_notices(${target_name})

    if(project_name STREQUAL "Showcase")
        if(product_role STREQUAL "editor")
            sparkle_configure_product_identity(${target_name} "Sparkle Editor")
        else()
            sparkle_configure_product_identity(${target_name} "Sparkle Game")
        endif()
    else()
        sparkle_configure_product_identity(${target_name} "${project_name} ${product_role}")
    endif()

    if(product_role STREQUAL "editor")
        if(NOT DEFINED SPARKLE_FONT_AWESOME_SOLID_TTF OR NOT EXISTS "${SPARKLE_FONT_AWESOME_SOLID_TTF}")
            message(FATAL_ERROR
                "Sparkle editor products require Font Awesome Free Solid. "
                "FetchDependencies.cmake must provide fa-solid-900.ttf before project products are configured.")
        endif()
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target_name}>/Fonts"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SPARKLE_FONT_AWESOME_SOLID_TTF}"
                "$<TARGET_FILE_DIR:${target_name}>/Fonts/fa-solid-900.ttf"
            COMMENT "Staging editor icon font for ${target_name}"
            VERBATIM
        )
    endif()

    if(WIN32)
        target_sources(${target_name} PRIVATE "${SPARKLE_WINDOWS_APPLICATION_MANIFEST}")
    endif()
endfunction()
