set(_sparkle_json_revision "55f93686c01528224f448c19128836e7df245f72")
sparkle_declare_git_dependency(nlohmann_json
    https://github.com/nlohmann/json.git
    ${_sparkle_json_revision})
FetchContent_Populate(nlohmann_json)
sparkle_prepare_git_fetchcontent_source(nlohmann_json
    EXPECTED_REVISION "${_sparkle_json_revision}"
    REQUIRED_PATHS CMakeLists.txt include/nlohmann/json.hpp include/nlohmann/json_fwd.hpp LICENSE.MIT)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("nlohmann/json" "${nlohmann_json_SOURCE_DIR}" "header-only")
    return()
endif()

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
set(JSON_CI OFF CACHE BOOL "" FORCE)
set(JSON_ImplicitConversions OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostics OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostic_Positions OFF CACHE BOOL "" FORCE)
set(JSON_LegacyDiscardedValueComparison OFF CACHE BOOL "" FORCE)
sparkle_add_dependency_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
set_property(TARGET nlohmann_json PROPERTY SPARKLE_SOURCE_PACKAGE
    "{\"name\":\"nlohmann/json\",\"versionInfo\":\"3.12.0\",\"licenseDeclared\":\"MIT\",\"downloadLocation\":\"https://github.com/nlohmann/json/archive/${_sparkle_json_revision}.tar.gz\"}")
