set(SPARKLE_KTX_REVISION 91ace88675ac59a97e55d0378a6602a9ae6b98bd)

set(_sparkle_ktx_required_paths CMakeLists.txt include/ktx.h lib/texture.c lib/texture1.c lib/texture2.c
    lib/basis_transcode.cpp lib/miniz_wrapper.cpp lib/basisu/encoder/basisu_miniz.h
    lib/basisu/zstd/zstd.c cmake/version.cmake include/KHR/khr_df.h
    LICENSE.md NOTICE.md LICENSES/Apache-2.0.txt LICENSES/BSD-1-Clause.txt LICENSES/LicenseRef-ETCSLA.txt lib/basisu/zstd/LICENSE)
sparkle_record_source_dependency(ktx VERSION 4.3.2 REVISION "${SPARKLE_KTX_REVISION}"
    PACKAGE_NAME "KTX-Software" LICENSE_ID "Apache-2.0 AND BSD-1-Clause AND BSD-3-Clause AND LicenseRef-ETCSLA"
    LOCATION "https://github.com/KhronosGroup/KTX-Software/tree/${SPARKLE_KTX_REVISION}"
    CACHE_DIRECTORY ktx-src REQUIRED_PATHS ${_sparkle_ktx_required_paths})
if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY)
    return()
endif()

# Reuse the owned Git cache with a sparse source checkout, without upstream test media.
set(_ktx_source "${FETCHCONTENT_BASE_DIR}/ktx-src")
if(NOT EXISTS "${_ktx_source}")
    execute_process(COMMAND "${GIT_EXECUTABLE}" clone --no-checkout --filter=blob:none --sparse
        https://github.com/KhronosGroup/KTX-Software.git "${_ktx_source}"
        RESULT_VARIABLE _ktx_result)
    if(NOT _ktx_result EQUAL 0)
        message(FATAL_ERROR "KTX clone was interrupted or failed. Cache preserved at '${_ktx_source}'. Explicitly Clean KTX before retrying.")
    endif()
    execute_process(COMMAND "${GIT_EXECUTABLE}" sparse-checkout set --no-cone
        "/*" "!/*/" "/include/" "/lib/" "!/lib/astc-encoder/Test/" "!/lib/basisu/webgl/"
        "!/lib/basisu/contrib/" "!/lib/basisu/test_files/" "/other_include/" "/cmake/" "/utils/"
        "/LICENSES/" "/tools/" "/interface/" "/tests/CMakeLists.txt"
        WORKING_DIRECTORY "${_ktx_source}" RESULT_VARIABLE _ktx_result)
    if(NOT _ktx_result EQUAL 0)
        message(FATAL_ERROR "KTX sparse checkout failed. Cache preserved at '${_ktx_source}'.")
    endif()
    execute_process(COMMAND "${GIT_EXECUTABLE}" checkout --detach "${SPARKLE_KTX_REVISION}"
        WORKING_DIRECTORY "${_ktx_source}" RESULT_VARIABLE _ktx_result)
    if(NOT _ktx_result EQUAL 0)
        message(FATAL_ERROR "KTX checkout failed. Cache preserved at '${_ktx_source}'.")
    endif()
endif()
sparkle_prepare_git_fetchcontent_source(ktx EXPECTED_REVISION "${SPARKLE_KTX_REVISION}"
    REQUIRED_PATHS ${_sparkle_ktx_required_paths})
set_property(GLOBAL APPEND PROPERTY SPARKLE_ADMITTED_GIT_DEPENDENCIES ktx)
set_property(GLOBAL PROPERTY SPARKLE_GIT_DEPENDENCY_ktx_ID ktx)
set(ktx_SOURCE_DIR "${_ktx_source}")
set(ktx_BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/ktx-build")
if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

# CMake 4.2 can reach CTestTargets before the fetched binary tree has created
# its CMakeFiles directory on repeated configure passes. Seed the sub-build
# directories explicitly so the KTX configure remains stable during cook flows.
file(MAKE_DIRECTORY "${ktx_BINARY_DIR}")
file(MAKE_DIRECTORY "${ktx_BINARY_DIR}/CMakeFiles")

# Use the upstream read-only target; the writer/encoders stay excluded from tool builds.
block(SCOPE_FOR VARIABLES)
    # Keep the selected load-test value when upstream declares its cache entry.
    set(CMAKE_POLICY_DEFAULT_CMP0126 NEW)
    set(KTX_FEATURE_STATIC_LIBRARY ON)
    set(KTX_FEATURE_TESTS OFF)
    set(KTX_FEATURE_DOC OFF)
    set(KTX_FEATURE_TOOLS OFF)
    set(KTX_FEATURE_JNI OFF)
    set(KTX_FEATURE_PY OFF)
    set(KTX_FEATURE_LOADTEST_APPS "")
    set(KTX_FEATURE_VK_UPLOAD OFF)
    set(KTX_FEATURE_GL_UPLOAD OFF)
    set(BASISU_SUPPORT_OPENCL OFF)
    sparkle_add_dependency_subdirectory(${ktx_SOURCE_DIR} ${ktx_BINARY_DIR} EXCLUDE_FROM_ALL)
endblock()

if(TARGET ktx)
    set_target_properties(ktx PROPERTIES FOLDER "ThirdParty/ktx")
endif()
if(TARGET ktx_read)
    set_target_properties(ktx_read PROPERTIES FOLDER "ThirdParty/ktx")
    # Upstream recognizes only the literal Debug profile. Map its target-local
    # optimization/definitions to Sparkle's Debug profiles without changing SDK sources.
    foreach(_property IN ITEMS COMPILE_OPTIONS COMPILE_DEFINITIONS INTERFACE_COMPILE_DEFINITIONS)
        get_target_property(_options ktx_read ${_property})
        if(_options)
            string(REPLACE "$<CONFIG:Debug>" "$<CONFIG:Debug,DebugEditor,DebugGame>" _options "${_options}")
            set_property(TARGET ktx_read PROPERTY ${_property} ${_options})
        endif()
    endforeach()
    sparkle_add_dependency_source_package(ktx_read ktx)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled Basis Universal" Apache-2.0 lib/basisu/LICENSE)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled miniz" Apache-2.0 lib/basisu/encoder/basisu_miniz.h)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled Zstandard" BSD-3-Clause lib/basisu/zstd/LICENSE)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled uthash" BSD-1-Clause lib/uthash.h)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled dfdutils" Apache-2.0 lib/dfdutils/LICENSES/Apache-2.0.txt)
    sparkle_add_bundled_source_package(ktx_read ktx "KTX bundled Ericsson ETC" LicenseRef-ETCSLA LICENSES/LicenseRef-ETCSLA.txt)
endif()
