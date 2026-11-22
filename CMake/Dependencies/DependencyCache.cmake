function(download_sparkle_editor_asset url output_path display_name expected_sha256)
    set_property(GLOBAL APPEND PROPERTY SPARKLE_ADMITTED_SOURCE_FILES "${expected_sha256}|${output_path}")
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
    if(EXISTS "${_temporary_path}")
        file(SHA256 "${_temporary_path}" temporary_hash)
        if(NOT temporary_hash STREQUAL expected_sha256)
            message(FATAL_ERROR "Incomplete ${display_name} download preserved at '${_temporary_path}'. Inspect it, then explicitly Clean and Sync.")
        endif()
        file(RENAME "${_temporary_path}" "${output_path}" NO_REPLACE)
        return()
    endif()
    message(STATUS "    Downloading ${display_name}...")
    file(DOWNLOAD "${url}" "${_temporary_path}"
        STATUS _download_status TLS_VERIFY ON SHOW_PROGRESS
        EXPECTED_HASH "SHA256=${expected_sha256}")
    list(GET _download_status 0 _download_code)
    list(GET _download_status 1 _download_message)
    if(NOT _download_code EQUAL 0)
        message(FATAL_ERROR "Failed to download ${display_name}: ${_download_message}")
    endif()
    file(RENAME "${_temporary_path}" "${output_path}")
endfunction()

function(sparkle_collect_missing_relative_paths root_path out_missing_paths)
    set(missing_paths "")
    foreach(relative_path IN LISTS ARGN)
        if(NOT EXISTS "${root_path}/${relative_path}" OR IS_DIRECTORY "${root_path}/${relative_path}")
            list(APPEND missing_paths "${relative_path}")
        else()
            file(SIZE "${root_path}/${relative_path}" size)
            if(size EQUAL 0)
                list(APPEND missing_paths "${relative_path}")
            endif()
        endif()
    endforeach()
    set(${out_missing_paths} "${missing_paths}" PARENT_SCOPE)
endfunction()

function(sparkle_prepare_git_fetchcontent_source dependency_name)
    set(oneValueArgs DISPLAY_NAME SOURCE_DIR EXPECTED_REVISION)
    set(multiValueArgs REQUIRED_PATHS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

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
        COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=all --ignore-submodules=none
        WORKING_DIRECTORY "${ARG_SOURCE_DIR}"
        RESULT_VARIABLE status_result
        OUTPUT_VARIABLE source_status
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    # Accept the historical zlib 1.3.1 source-header rename without rewriting it.
    # New builds configure a private source copy and never create this exception.
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

function(sparkle_add_source_package target_name package_name version license location)
    cmake_parse_arguments(ARG "" "CONDITION" "" ${ARGN})
    set(package "{\"name\":\"${package_name}\",\"versionInfo\":\"${version}\",\"licenseDeclared\":\"${license}\",\"downloadLocation\":\"${location}\"}")
    if(ARG_CONDITION)
        set(package "$<${ARG_CONDITION}:${package}>")
    endif()
    set_property(TARGET ${target_name} APPEND PROPERTY SPARKLE_SOURCE_PACKAGES "${package}")
    set_property(TARGET ${target_name} PROPERTY INTERFACE_SPARKLE_SOURCE_PACKAGES "$<TARGET_PROPERTY:${target_name},SPARKLE_SOURCE_PACKAGES>")
    set_property(TARGET ${target_name} APPEND PROPERTY TRANSITIVE_LINK_PROPERTIES SPARKLE_SOURCE_PACKAGES)
endfunction()

# Shipped helper executables and data do not necessarily appear in the link graph.
function(sparkle_target_source_package_dependencies target_name)
    foreach(dependency IN LISTS ARGN)
        set_property(TARGET ${target_name} APPEND PROPERTY SPARKLE_SOURCE_PACKAGES
            "$<TARGET_PROPERTY:${dependency},SPARKLE_SOURCE_PACKAGES>")
    endforeach()
endfunction()

function(sparkle_record_source_dependency dependency_name)
    cmake_parse_arguments(ARG "" "VERSION;REVISION;CACHE_DIRECTORY;PACKAGE_NAME;LICENSE_ID;LOCATION" "REQUIRED_PATHS;CACHE_FILES" ${ARGN})
    set(ARG_LICENSE "${ARG_LICENSE_ID}")
    foreach(field IN ITEMS VERSION CACHE_DIRECTORY PACKAGE_NAME LICENSE LOCATION REQUIRED_PATHS)
        if("${ARG_${field}}" STREQUAL "")
            message(FATAL_ERROR "${dependency_name} is missing its ${field} declaration.")
        endif()
    endforeach()
    get_property(dependencies GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCIES)
    if(NOT dependency_name IN_LIST dependencies)
        set_property(GLOBAL APPEND PROPERTY SPARKLE_SOURCE_DEPENDENCIES "${dependency_name}")
    endif()
    foreach(field IN ITEMS VERSION REVISION CACHE_DIRECTORY PACKAGE_NAME LICENSE LOCATION REQUIRED_PATHS CACHE_FILES)
        set_property(GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency_name}_${field} "${ARG_${field}}")
    endforeach()
endfunction()

function(sparkle_add_dependency_source_package target_name dependency_name)
    foreach(field IN ITEMS PACKAGE_NAME VERSION LICENSE LOCATION)
        get_property(${field} GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency_name}_${field})
        if("${${field}}" STREQUAL "")
            message(FATAL_ERROR "Missing ${field} for source dependency '${dependency_name}'.")
        endif()
    endforeach()
    sparkle_add_source_package(${target_name} "${PACKAGE_NAME}" "${VERSION}" "${LICENSE}" "${LOCATION}" ${ARGN})
endfunction()

# Patch only named build inputs. The final copy preserves timestamps when the
# effective source is unchanged; the admitted shared checkout remains read-only.
function(sparkle_prepare_patched_dependency_files source_dir binary_dir patch_file out_directory)
    set(input_root "${binary_dir}/patched-input")
    set(output_root "${binary_dir}/patched-source")
    foreach(path IN LISTS ARGN)
        configure_file("${source_dir}/${path}" "${input_root}/${path}" COPYONLY)
    endforeach()
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${source_dir}" rev-parse --absolute-git-dir
        OUTPUT_VARIABLE git_directory OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND "${GIT_EXECUTABLE}" "--git-dir=${git_directory}" "--work-tree=${input_root}" apply "${patch_file}"
        WORKING_DIRECTORY "${input_root}" RESULT_VARIABLE result ERROR_VARIABLE error)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Dependency patch failed against its admitted base: ${patch_file}: ${error}")
    endif()
    foreach(path IN LISTS ARGN)
        configure_file("${input_root}/${path}" "${output_root}/${path}" COPYONLY)
    endforeach()
    set(${out_directory} "${output_root}" PARENT_SCOPE)
endfunction()

# Bundled components retain their parent's admitted source identity and license inputs.
function(sparkle_add_bundled_source_package target_name dependency_name component license)
    get_property(directory GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency_name}_CACHE_DIRECTORY)
    get_property(revision GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency_name}_REVISION)
    get_property(location GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency_name}_LOCATION)
    sparkle_collect_missing_relative_paths("${FETCHCONTENT_BASE_DIR}/${directory}" missing ${ARGN})
    if(missing)
        message(FATAL_ERROR "Bundled ${component} source/license inputs are missing: ${missing}")
    endif()
    sparkle_add_source_package(${target_name} "${component}" "${revision}" "${license}" "${location}")
endfunction()

function(sparkle_declare_git_dependency dependency_name repository revision)
    cmake_parse_arguments(ARG "" "ID;VERSION;LICENSE_ID;PACKAGE_NAME;GIT_SUBMODULES_RECURSE" "REQUIRED_PATHS;GIT_SUBMODULES;GIT_CONFIG" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS OR NOT ARG_VERSION OR NOT ARG_LICENSE_ID OR NOT ARG_REQUIRED_PATHS)
        message(FATAL_ERROR "${dependency_name} requires version, license and consumed-file admission facts; unexpected arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_ID)
        set(ARG_ID "${dependency_name}")
    endif()
    if(NOT ARG_PACKAGE_NAME)
        set(ARG_PACKAGE_NAME "${dependency_name}")
    endif()
    string(REGEX REPLACE "[.]git$" "" location "${repository}")
    sparkle_record_source_dependency("${ARG_ID}"
        VERSION "${ARG_VERSION}" REVISION "${revision}" LICENSE_ID "${ARG_LICENSE_ID}"
        PACKAGE_NAME "${ARG_PACKAGE_NAME}" LOCATION "${location}/tree/${revision}"
        CACHE_DIRECTORY "${dependency_name}-src" REQUIRED_PATHS ${ARG_REQUIRED_PATHS})
    if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY)
        return()
    endif()
    set_property(GLOBAL APPEND PROPERTY SPARKLE_ADMITTED_GIT_DEPENDENCIES "${dependency_name}")
    set_property(GLOBAL PROPERTY SPARKLE_GIT_DEPENDENCY_${dependency_name}_ID "${ARG_ID}")
    set(git_options)
    foreach(option IN ITEMS GIT_SUBMODULES GIT_CONFIG GIT_SUBMODULES_RECURSE)
        if(DEFINED ARG_${option})
            list(APPEND git_options ${option} ${ARG_${option}})
        endif()
    endforeach()
    FetchContent_Declare(${dependency_name}
        SOURCE_SUBDIR sparkle-source-only
        BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/${dependency_name}-build"
        GIT_REPOSITORY "${repository}"
        GIT_TAG "${revision}"
        GIT_SHALLOW FALSE
        GIT_PROGRESS FALSE ${git_options})
    sparkle_prepare_git_fetchcontent_source("${dependency_name}" EXPECTED_REVISION "${revision}" REQUIRED_PATHS ${ARG_REQUIRED_PATHS})
    FetchContent_MakeAvailable(${dependency_name})
    # Check cold acquisition too, including consumed license/source files.
    sparkle_prepare_git_fetchcontent_source("${dependency_name}" EXPECTED_REVISION "${revision}" REQUIRED_PATHS ${ARG_REQUIRED_PATHS})
    set(${dependency_name}_SOURCE_DIR "${${dependency_name}_SOURCE_DIR}" PARENT_SCOPE)
    set(${dependency_name}_BINARY_DIR "${${dependency_name}_BINARY_DIR}" PARENT_SCOPE)
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/ArchiveDependency.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SourceDependencyAdmission.cmake")
