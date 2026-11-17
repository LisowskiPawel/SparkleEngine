set(_sparkle_cli11_revision "cbd58a3696887b34c70949aef21a71735a0c2ad5")
set(_sparkle_cli11_repository "https://github.com/CLIUtils/CLI11.git")
sparkle_declare_git_dependency(cli11
    ${_sparkle_cli11_repository}
    ${_sparkle_cli11_revision})
FetchContent_Populate(cli11)
sparkle_prepare_git_fetchcontent_source(cli11
    EXPECTED_REVISION "${_sparkle_cli11_revision}"
    REQUIRED_PATHS CMakeLists.txt include/CLI/CLI.hpp include/CLI/App.hpp include/CLI/impl/App_inl.hpp LICENSE)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("CLI11" "${cli11_SOURCE_DIR}" "header-only")
    return()
endif()

set(CLI11_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(CLI11_INSTALL OFF CACHE BOOL "" FORCE)
set(CLI11_PRECOMPILED OFF CACHE BOOL "" FORCE)
set(CLI11_MODULES OFF CACHE BOOL "" FORCE)
set(CLI11_SINGLE_FILE OFF CACHE BOOL "" FORCE)
set(CLI11_SANITIZERS OFF CACHE BOOL "" FORCE)
sparkle_add_dependency_subdirectory(${cli11_SOURCE_DIR} ${cli11_BINARY_DIR})
get_directory_property(_sparkle_cli11_version DIRECTORY "${cli11_SOURCE_DIR}" DEFINITION PROJECT_VERSION)
set_target_properties(CLI11 PROPERTIES FOLDER "ThirdParty")
target_compile_definitions(CLI11 INTERFACE CLI11_ENABLE_EXTRA_VALIDATORS=0)
set_property(TARGET CLI11 PROPERTY SPARKLE_SOURCE_PACKAGE
    "{\"name\":\"CLI11\",\"versionInfo\":\"${_sparkle_cli11_version}\",\"licenseDeclared\":\"BSD-3-Clause\",\"downloadLocation\":\"https://github.com/CLIUtils/CLI11/archive/${_sparkle_cli11_revision}.tar.gz\"}")
