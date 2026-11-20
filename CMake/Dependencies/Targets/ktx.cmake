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
    REQUIRED_PATHS CMakeLists.txt include/ktx.h lib/texture.c lib/texture1.c lib/texture2.c
    lib/basis_transcode.cpp lib/miniz_wrapper.cpp lib/basisu/encoder/basisu_miniz.h
    lib/basisu/zstd/zstd.c cmake/version.cmake include/KHR/khr_df.h
    LICENSE.md NOTICE.md LICENSES/Apache-2.0.txt LICENSES/BSD-1-Clause.txt LICENSES/LicenseRef-ETCSLA.txt lib/basisu/zstd/LICENSE)
set(ktx_SOURCE_DIR "${_ktx_source}")
set(ktx_BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/ktx-build")
if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

# Disable features we don't need
set(KTX_FEATURE_STATIC_LIBRARY ON  CACHE BOOL "" FORCE)
set(KTX_FEATURE_TESTS    OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_DOC      OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_TOOLS    OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_JNI      OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_PY       OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_LOADTEST_APPS "" CACHE STRING "" FORCE)
set(KTX_FEATURE_VK_UPLOAD OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_GL_UPLOAD OFF CACHE BOOL "" FORCE)
set(BASISU_SUPPORT_OPENCL OFF CACHE BOOL "" FORCE)

# CMake 4.2 can reach CTestTargets before the fetched binary tree has created
# its CMakeFiles directory on repeated configure passes. Seed the sub-build
# directories explicitly so the KTX configure remains stable during cook flows.
file(MAKE_DIRECTORY "${ktx_BINARY_DIR}")
file(MAKE_DIRECTORY "${ktx_BINARY_DIR}/CMakeFiles")

# Use the upstream read-only target; the writer/encoders stay excluded from tool builds.
sparkle_add_dependency_subdirectory(${ktx_SOURCE_DIR} ${ktx_BINARY_DIR} EXCLUDE_FROM_ALL)

if(TARGET ktx)
    set_target_properties(ktx PROPERTIES FOLDER "ThirdParty/KTX")
endif()
if(TARGET ktx_read)
    set_target_properties(ktx_read PROPERTIES FOLDER "ThirdParty/KTX")
    # Upstream recognizes only the literal Debug profile. Map its target-local
    # optimization/definitions to Sparkle's Debug profiles without changing SDK sources.
    foreach(_property IN ITEMS COMPILE_OPTIONS COMPILE_DEFINITIONS INTERFACE_COMPILE_DEFINITIONS)
        get_target_property(_options ktx_read ${_property})
        if(_options)
            string(REPLACE "$<CONFIG:Debug>" "$<CONFIG:Debug,DebugEditor,DebugGame>" _options "${_options}")
            set_property(TARGET ktx_read PROPERTY ${_property} ${_options})
        endif()
    endforeach()
    sparkle_set_source_package(ktx_read "KTX-Software" "4.3.2"
        "Apache-2.0 AND BSD-1-Clause AND BSD-3-Clause AND LicenseRef-ETCSLA" "https://github.com/KhronosGroup/KTX-Software/tree/${SPARKLE_KTX_REVISION}")
endif()
