set(_sparkle_spdlog_revision 27cb4c76708608465c413f6d0e6b8d99a4d84302)
sparkle_declare_git_dependency(spdlog
    https://github.com/gabime/spdlog.git
    ${_sparkle_spdlog_revision}
    ID spdlog VERSION "1.14.1" PACKAGE_NAME "spdlog" LICENSE_ID "MIT"
    REQUIRED_PATHS CMakeLists.txt include/spdlog/spdlog.h LICENSE include/spdlog/fmt/bundled/core.h include/spdlog/fmt/bundled/fmt.license.rst)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_spdlog)
    set(BUILD_SHARED_LIBS OFF)
    set(SPDLOG_INSTALL OFF)
    set(SPDLOG_BUILD_SHARED OFF)
    set(SPDLOG_BUILD_ALL OFF)
    set(SPDLOG_BUILD_EXAMPLE OFF)
    set(SPDLOG_BUILD_EXAMPLE_HO OFF)
    set(SPDLOG_BUILD_TESTS OFF)
    set(SPDLOG_BUILD_TESTS_HO OFF)
    set(SPDLOG_BUILD_BENCH OFF)
    set(SPDLOG_FMT_EXTERNAL OFF)
    set(SPDLOG_FMT_EXTERNAL_HO OFF)
    set(SPDLOG_USE_STD_FORMAT OFF)
    sparkle_add_dependency_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
    set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty/spdlog")
    target_compile_definitions(spdlog PUBLIC FMT_CONSTEVAL=constexpr)
endfunction()
sparkle_add_spdlog()

sparkle_add_dependency_source_package(spdlog spdlog)
sparkle_add_bundled_source_package(spdlog spdlog "spdlog bundled fmt" MIT include/spdlog/fmt/bundled/fmt.license.rst)
