set(_sparkle_cli11_revision "cbd58a3696887b34c70949aef21a71735a0c2ad5")
set(_sparkle_cli11_version "2.7.2")
set(_sparkle_cli11_repository "https://github.com/CLIUtils/CLI11.git")
sparkle_declare_git_dependency(cli11
    ${_sparkle_cli11_repository}
    ${_sparkle_cli11_revision}
    ID cli11 VERSION "${_sparkle_cli11_version}" PACKAGE_NAME "CLI11" LICENSE_ID "BSD-3-Clause"
    REQUIRED_PATHS CMakeLists.txt include/CLI/CLI.hpp include/CLI/App.hpp include/CLI/impl/App_inl.hpp LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

block(SCOPE_FOR VARIABLES)
    set(CLI11_BUILD_TESTS OFF)
    set(CLI11_BUILD_EXAMPLES OFF)
    set(CLI11_BUILD_DOCS OFF)
    set(CLI11_INSTALL OFF)
    set(CLI11_PRECOMPILED OFF)
    set(CLI11_MODULES OFF)
    set(CLI11_SINGLE_FILE OFF)
    set(CLI11_SANITIZERS OFF)
    sparkle_add_dependency_subdirectory(${cli11_SOURCE_DIR} ${cli11_BINARY_DIR})
endblock()
get_directory_property(_sparkle_cli11_actual_version DIRECTORY "${cli11_SOURCE_DIR}" DEFINITION PROJECT_VERSION)
if(NOT _sparkle_cli11_actual_version STREQUAL _sparkle_cli11_version)
    message(FATAL_ERROR "CLI11 source version '${_sparkle_cli11_actual_version}' differs from its declared identity '${_sparkle_cli11_version}'.")
endif()
set_target_properties(CLI11 PROPERTIES FOLDER "ThirdParty/cli11")
target_compile_definitions(CLI11 INTERFACE CLI11_ENABLE_EXTRA_VALIDATORS=0)

sparkle_add_dependency_source_package(CLI11 cli11)
