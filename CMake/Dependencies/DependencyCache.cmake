function(download_sparkle_editor_asset url output_path display_name expected_sha256)
    if(EXISTS "${output_path}")
        file(SHA256 "${output_path}" _asset_sha256)
        if(NOT _asset_sha256 STREQUAL expected_sha256)
            message(FATAL_ERROR "Cached ${display_name} fails its pinned SHA-256; preserved ${output_path} for inspection.")
        endif()
        return()
    endif()

    get_filename_component(_asset_dir "${output_path}" DIRECTORY)
    file(MAKE_DIRECTORY "${_asset_dir}")
    set(_temporary_path "${output_path}.download")
    message(STATUS "    Downloading ${display_name}...")
    file(DOWNLOAD "${url}" "${_temporary_path}"
        STATUS _download_status TLS_VERIFY ON SHOW_PROGRESS
        EXPECTED_HASH "SHA256=${expected_sha256}")
    list(GET _download_status 0 _download_code)
    list(GET _download_status 1 _download_message)
    if(NOT _download_code EQUAL 0)
        file(REMOVE "${_temporary_path}")
        message(FATAL_ERROR "Failed to download ${display_name}: ${_download_message}")
    endif()
    file(RENAME "${_temporary_path}" "${output_path}")
endfunction()

function(sparkle_collect_missing_relative_paths root_path out_missing_paths)
    set(missing_paths "")
    foreach(relative_path IN LISTS ARGN)
        if(NOT EXISTS "${root_path}/${relative_path}")
            list(APPEND missing_paths "${relative_path}")
        endif()
    endforeach()
    set(${out_missing_paths} "${missing_paths}" PARENT_SCOPE)
endfunction()

function(sparkle_prepare_git_fetchcontent_source dependency_name)
    set(oneValueArgs DISPLAY_NAME SOURCE_DIR EXPECTED_REVISION)
    set(multiValueArgs REQUIRED_PATHS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(LENGTH "${ARG_EXPECTED_REVISION}" revision_length)
    if(NOT revision_length EQUAL 40 OR NOT ARG_EXPECTED_REVISION MATCHES "^[0-9a-f]+$")
        message(FATAL_ERROR "${dependency_name} requires a full Git commit hash.")
    endif()
    if("${ARG_DISPLAY_NAME}" STREQUAL "")
        set(ARG_DISPLAY_NAME "${dependency_name}")
    endif()
    if("${ARG_SOURCE_DIR}" STREQUAL "")
        set(ARG_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/${dependency_name}-src")
    endif()

    string(TOUPPER "${dependency_name}" dependency_upper)
    set(source_cache_key "FETCHCONTENT_SOURCE_DIR_${dependency_upper}")
    if(NOT "${${source_cache_key}}" STREQUAL "" AND NOT "${${source_cache_key}}" STREQUAL "${ARG_SOURCE_DIR}")
        message(FATAL_ERROR "${ARG_DISPLAY_NAME} source override escapes its owned cache: '${${source_cache_key}}'. Expected '${ARG_SOURCE_DIR}'.")
    endif()
    if(NOT EXISTS "${ARG_SOURCE_DIR}")
        set(${source_cache_key} "" CACHE PATH "" FORCE)
        return()
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${ARG_SOURCE_DIR}"
        RESULT_VARIABLE revision_result
        OUTPUT_VARIABLE current_revision
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=all
        WORKING_DIRECTORY "${ARG_SOURCE_DIR}"
        RESULT_VARIABLE status_result
        OUTPUT_VARIABLE source_status
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    # zlib 1.3.1 moves its tracked header aside during out-of-source configure.
    # Admit only that exact rename, with the original Git blob still intact.
    if(dependency_name STREQUAL "zlib" AND source_status STREQUAL " D zconf.h" AND EXISTS "${ARG_SOURCE_DIR}/zconf.h.included")
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD:zconf.h
            WORKING_DIRECTORY "${ARG_SOURCE_DIR}"
            RESULT_VARIABLE original_result
            OUTPUT_VARIABLE original_header
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" hash-object --path=zconf.h zconf.h.included
            WORKING_DIRECTORY "${ARG_SOURCE_DIR}"
            RESULT_VARIABLE moved_result
            OUTPUT_VARIABLE moved_header
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(original_result EQUAL 0 AND moved_result EQUAL 0 AND original_header STREQUAL moved_header)
            set(source_status "")
        endif()
    endif()

    if(NOT EXISTS "${ARG_SOURCE_DIR}/.git" OR NOT revision_result EQUAL 0 OR NOT status_result EQUAL 0 OR
        NOT current_revision STREQUAL ARG_EXPECTED_REVISION OR NOT source_status STREQUAL "")
        message(FATAL_ERROR
            "${ARG_DISPLAY_NAME} cache is dirty, incomplete or not at ${ARG_EXPECTED_REVISION}: '${ARG_SOURCE_DIR}'. "
        "Cache preserved. Inspect it, then explicitly clean the dependency cache and retry Sync Code.")
    endif()
    sparkle_collect_missing_relative_paths("${ARG_SOURCE_DIR}" missing_paths ${ARG_REQUIRED_PATHS})
    if(missing_paths)
        message(FATAL_ERROR "${ARG_DISPLAY_NAME} cache is incomplete: ${missing_paths}. Cache preserved at '${ARG_SOURCE_DIR}'.")
    endif()
    set(${source_cache_key} "${ARG_SOURCE_DIR}" CACHE PATH "" FORCE)
endfunction()

function(sparkle_declare_git_dependency dependency_name repository revision)
    FetchContent_Declare(${dependency_name}
        GIT_REPOSITORY "${repository}"
        GIT_TAG "${revision}"
        GIT_SHALLOW FALSE
        GIT_PROGRESS ${_sparkle_git_progress})
    sparkle_prepare_git_fetchcontent_source("${dependency_name}" EXPECTED_REVISION "${revision}")
endfunction()

function(sparkle_ensure_archive_dependency)
    set(options)
    set(oneValueArgs DISPLAY_NAME URL ARCHIVE_PATH ROOT_PATH EXPECTED_HASH)
    set(multiValueArgs REQUIRED_PATHS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    sparkle_collect_missing_relative_paths("${ARG_ROOT_PATH}" missing_paths ${ARG_REQUIRED_PATHS})
    if(missing_paths)
        file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")
        file(DOWNLOAD
            "${ARG_URL}"
            "${ARG_ARCHIVE_PATH}"
            EXPECTED_HASH "${ARG_EXPECTED_HASH}"
            STATUS download_status
            TLS_VERIFY ON
            SHOW_PROGRESS
        )
        list(GET download_status 0 download_code)
        list(GET download_status 1 download_message)
        if(NOT download_code EQUAL 0)
            file(REMOVE "${ARG_ARCHIVE_PATH}")
            message(FATAL_ERROR "Failed to download ${ARG_DISPLAY_NAME}: ${download_message}")
        endif()

        file(REMOVE_RECURSE "${ARG_ROOT_PATH}")
        file(MAKE_DIRECTORY "${ARG_ROOT_PATH}")
        file(ARCHIVE_EXTRACT INPUT "${ARG_ARCHIVE_PATH}" DESTINATION "${ARG_ROOT_PATH}")

        sparkle_collect_missing_relative_paths("${ARG_ROOT_PATH}" missing_paths ${ARG_REQUIRED_PATHS})
    endif()

    if(missing_paths)
        list(JOIN missing_paths ", " missing_paths_text)
        message(FATAL_ERROR "${ARG_DISPLAY_NAME} recovery is incomplete. Missing: ${missing_paths_text} under '${ARG_ROOT_PATH}'.")
    endif()
endfunction()
