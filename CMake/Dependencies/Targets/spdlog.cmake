sparkle_declare_git_dependency(spdlog
    https://github.com/gabime/spdlog.git
    27cb4c76708608465c413f6d0e6b8d99a4d84302)
FetchContent_Populate(spdlog)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

function(sparkle_add_spdlog)
    set(BUILD_SHARED_LIBS OFF)
    sparkle_add_dependency_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
    set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty")
    target_compile_definitions(spdlog PUBLIC FMT_CONSTEVAL=constexpr)
endfunction()
sparkle_add_spdlog()
