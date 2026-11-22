# Runtime files and their distribution notices are staged together.
function(sparkle_record_runtime_targets directory)
    get_property(targets DIRECTORY "${directory}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        get_target_property(type ${target} TYPE)
        if(type MATCHES "^(SHARED_LIBRARY|MODULE_LIBRARY)$")
            set_property(TARGET ${target} PROPERTY INTERFACE_SPARKLE_RUNTIME_TARGETS "${target}")
        endif()
    endforeach()
    get_property(subdirectories DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
    foreach(subdirectory IN LISTS subdirectories)
        sparkle_record_runtime_targets("${subdirectory}")
    endforeach()
endfunction()
cmake_language(DEFER CALL sparkle_record_runtime_targets "${CMAKE_SOURCE_DIR}")

function(sparkle_stage_runtime_files product_target owner_name)
    cmake_parse_arguments(ARG "" "DESTINATION_DIRECTORY" "FILES;DEPENDS;STALE_FILES;PRESERVE_FILES" ${ARGN})
    set(staging_name "${product_target}${owner_name}Runtime")
    set(staging_root "${CMAKE_CURRENT_BINARY_DIR}/runtime-staging/${staging_name}")
    set(manifest "${staging_root}-$<CONFIG>.cmake")
    set(destination "${ARG_DESTINATION_DIRECTORY}")
    if(NOT destination)
        set(destination "$<TARGET_FILE_DIR:${product_target}>")
    endif()
    file(GENERATE OUTPUT "${manifest}" CONTENT
        "set(SPARKLE_RUNTIME_DIRECTORY [==[${destination}]==])\nset(SPARKLE_RUNTIME_FILES [==[${ARG_FILES}]==])\nset(SPARKLE_RUNTIME_STALE_FILES [==[${ARG_STALE_FILES}]==])\nset(SPARKLE_RUNTIME_PRESERVE_FILES [==[${ARG_PRESERVE_FILES}]==])\nset(SPARKLE_RUNTIME_RECORD [==[${staging_root}-$<CONFIG>.files]==])\n")
    add_custom_target(${staging_name}
        COMMAND "${SPARKLE_POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass
            -File "${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/SourceCacheRead.ps1"
            "${SPARKLE_SOURCE_CACHE_LOCK_FILE}" ${CMAKE_COMMAND} "-DSPARKLE_RUNTIME_MANIFEST=${manifest}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/StageRuntimeFiles.cmake"
        DEPENDS ${ARG_DEPENDS}
        VERBATIM)
    set_target_properties(${staging_name} PROPERTIES FOLDER "Runtime" SPARKLE_READS_SOURCE_CACHE TRUE)
    add_dependencies(${product_target} ${staging_name})
endfunction()

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
        sparkle_stage_runtime_files(${target_name} Msvc FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        set_property(TARGET ${target_name} PROPERTY
            SPARKLE_MSVC_RUNTIME_FILES "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}")
        set(runtime_identity)
        foreach(input IN LISTS CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${input}")
            file(SHA256 "${input}" hash)
            string(APPEND runtime_identity "${hash}\n")
        endforeach()
        string(SHA256 runtime_identity "${runtime_identity}")
        sparkle_add_source_package(${target_name} "Microsoft Visual C++ runtime" "sha256:${runtime_identity}"
            LicenseRef-Microsoft-Visual-Cpp-Runtime
            "https://visualstudio.microsoft.com/license-terms/vs2026-ga-visualcpp-v14-redist-runtime/")
    endif()
endfunction()

function(sparkle_stage_linked_runtime product_target)
    cmake_parse_arguments(ARG "" "EXCLUDE_REGEX" "" ${ARGN})
    set(runtime_files "$<TARGET_RUNTIME_DLLS:${product_target}>")
    set(preserved_files)
    if(ARG_EXCLUDE_REGEX)
        # Transfer files to their explicit owner without deleting its concurrent staging.
        set(preserved_files "$<PATH:GET_FILENAME,$<FILTER:${runtime_files},INCLUDE,${ARG_EXCLUDE_REGEX}>>")
        set(runtime_files "$<FILTER:${runtime_files},EXCLUDE,${ARG_EXCLUDE_REGEX}>")
    endif()
    set_property(TARGET ${product_target} APPEND PROPERTY TRANSITIVE_LINK_PROPERTIES SPARKLE_RUNTIME_TARGETS)
    set_property(TARGET ${product_target} APPEND PROPERTY SPARKLE_RUNTIME_SUPPORT_FILES
        "$<PATH:GET_FILENAME,$<TARGET_RUNTIME_DLLS:${product_target}>>")
    sparkle_stage_runtime_files(${product_target} Linked
        FILES "${runtime_files}"
        PRESERVE_FILES "${preserved_files}"
        DEPENDS "$<TARGET_GENEX_EVAL:${product_target},$<TARGET_PROPERTY:${product_target},SPARKLE_RUNTIME_TARGETS>>")
endfunction()
