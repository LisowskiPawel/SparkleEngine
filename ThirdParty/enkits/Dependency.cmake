set(SPARKLE_ENKITS_REVISION "0289cf6ffce86697e1f46cb4eeca1c946bffec71")
set(SPARKLE_ENKITS_PATCH "ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch")
set(SPARKLE_ENKITS_PATCH_SHA256 "8d066c5a4227d6943a7f3cd68176e5f5c2791e257875ef194e0e9ab43d46b201")
set(SPARKLE_ENKITS_VERSION "1.12+sparkle.runtime.${SPARKLE_ENKITS_PATCH_SHA256}")
set(SPARKLE_ENKITS_REQUIRED_PATHS
    CMakeLists.txt License.txt README.md src/TaskScheduler.h src/LockLessMultiReadPipe.h
    src/TaskScheduler.cpp src/TaskScheduler_c.h src/TaskScheduler_c.cpp)

set(_sparkle_enkits_patch "${SPARKLE_REPOSITORY_ROOT}/${SPARKLE_ENKITS_PATCH}")
if(NOT EXISTS "${_sparkle_enkits_patch}")
    message(FATAL_ERROR "The qualified enkiTS runtime patch is missing: '${_sparkle_enkits_patch}'.")
endif()
file(SHA256 "${_sparkle_enkits_patch}" _sparkle_enkits_patch_sha256)
if(NOT _sparkle_enkits_patch_sha256 STREQUAL SPARKLE_ENKITS_PATCH_SHA256)
    message(FATAL_ERROR "enkiTS runtime patch differs from the qualified identity. Patch and cache preserved.")
endif()

sparkle_declare_git_dependency(enkits https://github.com/dougbinks/enkiTS.git
    ${SPARKLE_ENKITS_REVISION}
    ID enkits VERSION "${SPARKLE_ENKITS_VERSION}" PACKAGE_NAME "enkiTS (Sparkle runtime fork)" LICENSE_ID "Zlib"
    REQUIRED_PATHS ${SPARKLE_ENKITS_REQUIRED_PATHS} GIT_CONFIG core.autocrlf=false)
if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_enkits)
    set(_source "${enkits_BINARY_DIR}/source")
    if(IS_SYMLINK "${_source}" OR IS_SYMLINK "${_source}/src")
        message(FATAL_ERROR "enkiTS derived source must be an owned directory, not a link: '${_source}'.")
    endif()

    # Only the pinned target's source/build/license closure is needed. The
    # canonical Git cache remains pristine and the derived files are checked on
    # every configure; a stamp alone never admits modified source.
    foreach(_file IN LISTS SPARKLE_ENKITS_REQUIRED_PATHS)
        # The qualified Windows source/patch use CRLF. Derive those exact bytes
        # independently of Git's cache checkout policy, without changing cache.
        file(READ "${enkits_SOURCE_DIR}/${_file}" _contents)
        string(REPLACE "\r\n" "\n" _contents "${_contents}")
        string(REPLACE "\n" "\r\n" _windows_contents "${_contents}")
        string(SHA256 _hash "${_windows_contents}")
        if(_file STREQUAL "src/TaskScheduler.cpp")
            set(_hash "ca734db4fe54f5f13702b1c4c6103aa0c5fe42b6948fe77506fadae1f0987373")
        elseif(_file STREQUAL "src/TaskScheduler_c.cpp")
            set(_hash "5388245a1a6054260b6786558a5f9813c63c0c1c375f42e6f12f1c4a9e53d820")
        elseif(_file STREQUAL "src/TaskScheduler_c.h")
            set(_hash "6269a524d48c21c741c0c92fb06675beb81b7f015881c727e58c1a5add50c1a3")
        endif()
        string(MAKE_C_IDENTIFIER "${_file}" _key)
        set(_expected_${_key} "${_hash}")
        set(_contents_${_key} "${_contents}")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
            "${enkits_SOURCE_DIR}/${_file}" "${_source}/${_file}")
    endforeach()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_sparkle_enkits_patch}")

    if(NOT EXISTS "${_source}")
        foreach(_file IN LISTS SPARKLE_ENKITS_REQUIRED_PATHS)
            get_filename_component(_directory "${_source}/${_file}" DIRECTORY)
            file(MAKE_DIRECTORY "${_directory}")
            string(MAKE_C_IDENTIFIER "${_file}" _key)
            file(WRITE "${_source}/${_file}" "${_contents_${_key}}")
        endforeach()
        file(RELATIVE_PATH _relative_source "${SPARKLE_REPOSITORY_ROOT}" "${_source}")
        if(IS_ABSOLUTE "${_relative_source}" OR _relative_source MATCHES "^\\.\\.([/\\\\]|$)")
            message(FATAL_ERROR "enkiTS derived source escapes the workspace: '${_source}'.")
        endif()
        execute_process(COMMAND "${GIT_EXECUTABLE}" apply --check "--directory=${_relative_source}" "${_sparkle_enkits_patch}"
            WORKING_DIRECTORY "${SPARKLE_REPOSITORY_ROOT}" RESULT_VARIABLE _patch_check ERROR_VARIABLE _patch_error)
        if(NOT _patch_check EQUAL 0)
            message(FATAL_ERROR "enkiTS patch cannot apply to derived sources: ${_patch_error}. Sources preserved.")
        endif()
        execute_process(COMMAND "${GIT_EXECUTABLE}" apply "--directory=${_relative_source}" "${_sparkle_enkits_patch}"
            WORKING_DIRECTORY "${SPARKLE_REPOSITORY_ROOT}" RESULT_VARIABLE _patch_result ERROR_VARIABLE _patch_error)
        if(NOT _patch_result EQUAL 0)
            message(FATAL_ERROR "enkiTS patch failed: ${_patch_error}. Sources preserved.")
        endif()
    endif()

    file(GLOB_RECURSE _actual_files LIST_DIRECTORIES FALSE RELATIVE "${_source}" "${_source}/*")
    set(_expected_files ${SPARKLE_ENKITS_REQUIRED_PATHS})
    list(SORT _actual_files)
    list(SORT _expected_files)
    if(NOT _actual_files STREQUAL _expected_files)
        message(FATAL_ERROR "enkiTS derived sources are incomplete or contain extra files. Preserved '${_source}'; clean this owned variant build and retry.")
    endif()
    foreach(_file IN LISTS _expected_files)
        if(IS_SYMLINK "${_source}/${_file}")
            message(FATAL_ERROR "enkiTS derived file is a link: '${_source}/${_file}'.")
        endif()
        file(SHA256 "${_source}/${_file}" _actual_hash)
        string(MAKE_C_IDENTIFIER "${_file}" _key)
        if(NOT _actual_hash STREQUAL _expected_${_key})
            message(FATAL_ERROR "enkiTS derived source differs from the qualified bytes: '${_source}/${_file}'. Source preserved.")
        endif()
    endforeach()

    set(ENKITS_BUILD_C_INTERFACE ON)
    set(ENKITS_BUILD_EXAMPLES OFF)
    set(ENKITS_BUILD_SHARED ${SPARKLE_BUILD_SHARED})
    set(ENKITS_INSTALL OFF)
    set(ENKITS_SANITIZE OFF)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    sparkle_add_dependency_subdirectory("${_source}" "${enkits_BINARY_DIR}/native" EXCLUDE_FROM_ALL)
    if(MSVC)
        set_property(SOURCE "${_source}/src/TaskScheduler_c.cpp"
            TARGET_DIRECTORY enkiTS APPEND PROPERTY COMPILE_OPTIONS /EHc-)
    endif()
    set_target_properties(enkiTS PROPERTIES FOLDER "ThirdParty/enkits"
        MSVC_RUNTIME_LIBRARY "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    sparkle_configure_runtime_support_artifacts(enkiTS)
    sparkle_add_dependency_source_package(enkiTS enkits)
endfunction()
sparkle_add_enkits()
